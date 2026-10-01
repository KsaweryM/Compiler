# The source language

## Grammar

```
program      -> VAR declarations BEGIN commands END
              | BEGIN commands END

declarations -> declarations , pidentifier
              | declarations , pidentifier [ num : num ]
              | pidentifier
              | pidentifier [ num : num ]

commands     -> commands command
              | command

command      -> identifier ASSIGN expression ;
              | IF condition THEN commands ELSE commands ENDIF
              | IF condition THEN commands ENDIF
              | WHILE condition DO commands ENDWHILE
              | REPEAT commands UNTIL condition ;
              | FOR pidentifier FROM value TO value DO commands ENDFOR
              | FOR pidentifier FROM value DOWNTO value DO commands ENDFOR
              | READ identifier ;
              | WRITE value ;

expression   -> value
              | value PLUS value
              | value MINUS value
              | value TIMES value
              | value DIV value
              | value MOD value

condition    -> value EQ value
              | value NEQ value
              | value LE value
              | value GE value
              | value LEQ value
              | value GEQ value

value        -> num
              | identifier

identifier   -> pidentifier
              | pidentifier [ pidentifier ]
              | pidentifier [ num ]
```

Expressions and conditions have at most one operator; longer computations go through
variables.

## Lexical structure

- **Keywords** are written in capital letters: `VAR`, `BEGIN`, `ASSIGN`, `PLUS`, `EQ`, ...
- **Identifiers** (`pidentifier`) consist of letters and underscores: `n`, `sieve`, `max_value`.
  Digits are not allowed, so `a1` is the identifier `a` followed by the number `1`.
- **Numbers** (`num`) are 64-bit signed integers, written in decimal with an optional minus
  sign: `0`, `-17`, `1234567890`.
- **Comments** are enclosed in parentheses, may span several lines and cannot be nested:
  `( this is a comment )`.
- Spaces, tabs and line breaks (LF or CR LF) separate tokens.

## Semantics

### Variables and arrays

All variables hold 64-bit signed integers. Scalars are declared by name, arrays with an
inclusive range of indices, which may be negative: `t[-10:10]` has 21 elements. Arrays may
be very large – memory is allocated only for the elements that are written.

A scalar may be read only after it has been given a value by `ASSIGN` or `READ` earlier in
the program text. The check is textual, not dynamic: in

```
IF n GE 0 THEN x ASSIGN 1; ENDIF
WRITE x;
```

the use of `x` is accepted, and `x` is 0 if the assignment did not run. Array elements are
not checked; elements that were never written are 0.

### Arithmetic

| Expression | Value |
|---|---|
| `a PLUS b`, `a MINUS b`, `a TIMES b` | sum, difference and product |
| `a DIV b` | quotient rounded toward minus infinity: `-7 DIV 2 = -4` |
| `a MOD b` | remainder with the sign of the divisor, so that `a = (a DIV b) * b + a MOD b`: `-7 MOD 2 = 1`, `7 MOD -2 = -1` |
| `a DIV 0`, `a MOD 0` | 0 |

The result of an operation that does not fit in 64 bits is not specified.

### Conditions

`EQ` (=), `NEQ` (≠), `LE` (<), `GE` (>), `LEQ` (≤), `GEQ` (≥).

### Loops

- `WHILE` checks its condition before every iteration and may run zero times.
- `REPEAT ... UNTIL` checks its condition after every iteration and stops when it holds;
  the body runs at least once.
- `FOR i FROM a TO b` runs for `i = a, a+1, ..., b`; with `DOWNTO` for `i = a, a-1, ..., b`.
  Both bounds are evaluated once, before the first iteration – changing the variables they
  came from does not change the number of iterations. If the range is empty, the body
  does not run.

The iterator of a `FOR` loop is declared by the loop itself and exists only inside it. It
cannot be modified (by `ASSIGN` or `READ`). It may reuse the name of a declared variable or
of the iterator of an enclosing loop; the name then refers to the new iterator until
`ENDFOR`, and the hidden variable keeps its value.

### Input and output

`READ x;` reads a number from the standard input into `x`, `WRITE v;` prints a value.

## Errors

| Error | Example |
|---|---|
| syntax error | `syntax error, unexpected WRITE, expecting ;` |
| character outside the language | `unexpected character '+'` |
| comment that is never closed | `unterminated comment` |
| number that does not fit in 64 bits | `number 99999999999999999999 is out of range` |
| name declared twice | `'a' is already declared` |
| array with first index greater than last | `array 'b' has an invalid range [11:10]` |
| undeclared name | `'a' is not declared` |
| array used without an index | `'b' is an array and must be indexed` |
| scalar used with an index | `'a' is not an array` |
| constant index outside the array | `index 10 is out of bounds for array 't' [0:9]` |
| scalar read before getting a value | `'a' is used before being initialized` |
| loop iterator modified | `cannot modify loop iterator 'i'` |

Indices given by variables are not checked; an index outside the array accesses another
variable's memory.
