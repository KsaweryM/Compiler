# How the compiler works

The compiler runs four stages, each in its own directory under `src/`:

```
source ──► frontend ──► semantics ──► memory layout ──► backend ──► VM assembly
           lexer,        name           addresses of      code
           parser, AST   resolution,    all variables     generation
                         checks
```

1. **Frontend** (`src/frontend`). The Flex lexer and the Bison parser build an abstract
   syntax tree (`Ast.h`). Grammar actions only construct tree nodes; a syntax error stops
   the compilation.
2. **Semantic analysis** (`src/semantics/Analyzer.cpp`). Walks the tree in source order,
   binds every identifier to a `Symbol` and reports every error it finds (see
   [language.md](language.md#errors)). `FOR` loops open a new scope with their iterator,
   so an iterator can hide a variable of the same name. The analyzer also estimates how
   often each symbol is accessed: an access inside *d* nested loops counts as 8<sup>d</sup>.
3. **Memory layout** (`src/semantics/MemoryLayout.cpp`). Scalars get the lowest addresses,
   most frequently used first, because small addresses are the cheapest to build (see
   below). Arrays follow, smallest first.
4. **Code generation** (`src/backend`). `CodeGenerator` emits instructions into `Code`,
   which resolves jumps to labels into relative offsets when the program is written out.

## The virtual machine

The machine has eight registers `a`–`h` and memory cells addressed by non-negative
integers. All values are 64-bit signed integers. Registers start with unspecified values;
memory cells start with 0.

| Instruction | Effect | Cost |
|---|---|---|
| `GET` | read a number into `a` | 100 |
| `PUT` | print `a` | 100 |
| `LOAD x` | `a ← memory[x]` | 50 |
| `STORE x` | `memory[x] ← a` | 50 |
| `ADD x` | `a ← a + x` | 10 |
| `SUB x` | `a ← a − x` | 10 |
| `SHIFT x` | `a ← ⌊a · 2^x⌋` (a negative `x` divides, rounding down) | 5 |
| `SWAP x` | exchange `a` and `x` | 1 |
| `RESET x` | `x ← 0` | 1 |
| `INC x`, `DEC x` | `x ← x ± 1` | 1 |
| `JUMP j` | jump `j` instructions forward (or back, if negative) | 1 |
| `JPOS j`, `JZERO j`, `JNEG j` | jump if `a > 0`, `a = 0`, `a < 0` | 1 |
| `HALT` | stop | 0 |

The cost of a run is the sum of the costs of executed instructions. Memory access is by far
the most expensive operation, so the generated code keeps values in registers whenever it
can, and `Code` chooses between alternative instruction sequences using this cost table
(`cost()` in `src/backend/Code.cpp`).

## Code generation

### Registers

| Register | Role |
|---|---|
| `a` | accumulator: every value is computed here, and `LOAD a` reads the cell whose address is in `a` |
| `b` | scratch: building constants and array addresses |
| `c` | the right operand of a binary operation or a comparison |
| `c`–`h` | temporaries of the multiplication and division routines |
| `h` | address of the cell written by an assignment, `READ` or a loop update |

No value is kept in a register from one command to the next, so every command starts by
loading what it needs from memory.

### Constants

Constants (including addresses) are built from `RESET` and `INC`/`DEC`. For larger numbers
the leading bits are built this way and the remaining bits are appended one at a time:
double the value, then `INC` if the bit is set. Doubling is either `ADD a` (cost 10) or
`SHIFT` by a register holding 1 (cost 5, after 2 to prepare that register).
`Code::setConstant` computes the cost of every variant and picks the cheapest. For
example, 1000 = 0b1111101000 is built from 7 = 0b111 and seven more bits, at a cost of 48:

```
RESET b, INC b          b = 1
RESET a, INC a ×7       a = 7
SHIFT b, INC a          a = 15
SHIFT b, INC a          a = 31
SHIFT b                 a = 62
SHIFT b, INC a          a = 125
SHIFT b ×3              a = 250, 500, 1000
```

### Commands

A typical command loads its operands into `a` and `c`, operates, then stores the result
through `h`. Compiling

```
VAR s, t[1:10]
BEGIN
    s ASSIGN 0;
    FOR i FROM 1 TO 10 DO
        t[i] ASSIGN i TIMES 3;
        s ASSIGN s PLUS t[i];
    ENDFOR
    WRITE s;
END
```

places `i` at address 0, `s` at 1, the hidden loop bound at 2 and `t` at 3–12, and gives:

```
 0  RESET a                 s ASSIGN 0
 1  RESET h
 2  INC h
 3  STORE h
 4  RESET a                 i ASSIGN 1
 5  INC a
 6  RESET h
 7  STORE h
 8  JUMP 34                 go to the loop condition
 9  RESET a                 t[i] ASSIGN i TIMES 3: load i
10  LOAD a
11  SWAP d                  a = 3i, by shifting and adding
12  RESET a
13  ADD d
14  RESET c
15  INC c
16  SHIFT c
17  ADD d
18  SWAP h                  park the result in h
19  RESET a                 address of t[i] = i + 2
20  LOAD a
21  INC a
22  INC a
23  SWAP h
24  STORE h
25  RESET a                 s ASSIGN s PLUS t[i]: load t[i] into c
26  LOAD a
27  INC a
28  INC a
29  LOAD a
30  SWAP c
31  RESET a                 load s
32  INC a
33  LOAD a
34  ADD c
35  RESET h
36  INC h
37  STORE h
38  RESET h                 i ASSIGN i + 1
39  LOAD h
40  INC a
41  STORE h
42  DEC a ×10               loop condition: a = i - 10
52  JNEG -43               continue while i - 10 <= 0
53  JZERO -44
54  RESET a                 WRITE s
55  INC a
56  LOAD a
57  PUT
58  HALT
```

Other choices made by the generator:

- **Conditions** compute `left − right` in `a` and jump on its sign, directly to the target
  of the statement; no 0/1 value is produced.
- **`WHILE`** places its condition after the body (`JUMP check; body: ...; check: if
  condition then JUMP body`), which saves one jump per iteration.
- **`FOR`** keeps the iterator in memory and compares it with the final value after every
  increment. A constant final value is built into the comparison (as `DEC a ×10` above);
  any other final value is computed once, stored in a hidden cell and loaded every time.
- **Constant operands** are folded when both are known (`6 TIMES -7`), and conditions with
  a known outcome (`1 EQ 1`, `x LEQ x`) choose their branch at compile time.
- **Same operand twice**: `x PLUS x` is `ADD a`, `x MINUS x` and `x MOD x` are 0.

### Arithmetic

Multiplication and division are implemented in `src/backend/Arithmetic.cpp`:

- **Multiplication** by a constant uses shifts and additions along the bits of the constant;
  by a power of two, a single `SHIFT`. A general multiplication takes the absolute values,
  swaps the factors so that the smaller one drives the loop, and adds the larger factor
  shifted by *k* for every bit *k* of the smaller one. It runs in time proportional to the
  number of bits of the smaller factor.
- **Division** by a power of two is a single `SHIFT` by a negative amount, which rounds
  toward minus infinity exactly like `DIV`; `MOD` by a power of two is `x − (x / 2^k) · 2^k`.
  General division is binary long division on absolute values: the divisor is doubled until
  it exceeds the dividend (counting the doublings), then halved the same number of times,
  producing one bit of the quotient per halving. Finally the quotient and the remainder are
  corrected for the signs of the operands.

## Possible improvements

The biggest remaining cost is memory traffic: every command reloads its operands, and loop
iterators live in memory. The hand-written reference programs in `vm/examples/` show the
potential – the sieve costs 32 477 there and about 116 000 when compiled. Keeping loop
iterators and frequently used variables in registers across commands would require a
register allocator with liveness analysis on top of the current tree-based generator.
