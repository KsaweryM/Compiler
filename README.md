# Compiler

A compiler for a small imperative language, originally written as a university project
(Formal Languages and Translation Techniques) and later rebuilt. It translates programs into
the assembly language of a simple register machine, whose interpreter is included in `vm/`.

```
( Sieve of Eratosthenes )
VAR
    n, j, sieve[2:100]
BEGIN
    n ASSIGN 100;
    FOR i FROM n DOWNTO 2 DO
        sieve[i] ASSIGN 1;
    ENDFOR
    FOR i FROM 2 TO n DO
        IF sieve[i] NEQ 0 THEN
            j ASSIGN i PLUS i;
            WHILE j LEQ n DO
                sieve[j] ASSIGN 0;
                j ASSIGN j PLUS i;
            ENDWHILE
            WRITE i;
        ENDIF
    ENDFOR
END
```

## Building

Requirements: a C++17 compiler (GCC 8+ or Clang 7+), GNU Bison 3.2+, Flex and GNU Make.
Python 3 is needed only for `make fuzz`. On Debian or Ubuntu:

```sh
sudo apt install g++ make bison flex python3
make            # builds build/kompilator
make debug      # builds build/debug/kompilator with AddressSanitizer and UBSan
make help       # lists all targets
```

## Usage

```sh
build/kompilator program.imp program.mr     # compile
vm/vm program.mr                            # run on the virtual machine
make run PROGRAM=tests/programs/examples/sieve.imp
```

Errors are reported in the usual `file:line: error: message` format. Syntax errors stop the
compilation; all semantic errors are reported together. The exit status is 0 on success,
1 when the program contains errors and 2 on wrong usage.

```
tests/errors/multiple_errors.imp:10: error: 'x' is not declared
tests/errors/multiple_errors.imp:11: error: 'a' is used before being initialized
tests/errors/multiple_errors.imp:12: error: 't' is an array and must be indexed
```

## Testing

```sh
make test       # compiles every test program, runs it on the VM and checks the output
make fuzz       # compares the compiler with a reference interpreter on random programs
```

`make test` also prints the execution cost of every program, which makes it easy to see
the effect of a change in code generation. See [tests/README.md](tests/README.md) for how
tests are written.

## Project layout

| Path | Contents |
|---|---|
| [src/frontend/](src/frontend/) | lexer (Flex), parser (Bison) and the abstract syntax tree |
| [src/semantics/](src/semantics/) | name resolution, error checking and memory layout |
| [src/backend/](src/backend/) | code generation for the virtual machine |
| [src/driver/](src/driver/) | the `kompilator` command |
| [src/common/](src/common/) | error type shared by all stages |
| [tests/](tests/) | test programs, test runner and fuzzer |
| [docs/](docs/) | language reference and description of the compiler |
| [vm/](vm/) | the virtual machine and hand-written example programs |

## Documentation

- [docs/language.md](docs/language.md) – the source language: syntax, semantics and errors
- [docs/architecture.md](docs/architecture.md) – how the compiler works and what code it generates
- [vm/README.md](vm/README.md) – the virtual machine
