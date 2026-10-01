# Tests

```
tests/
  programs/      programs that must compile and print the expected output
    basics/        syntax: comments, optional declarations, line endings
    arithmetic/    operators, division semantics, constant operands
    control_flow/  conditions, IF, WHILE, REPEAT
    loops/         FOR loops: ranges, nesting, iterator scope
    arrays/        array indexing, huge and negative ranges
    examples/      complete algorithms (sieve, sorting, factorization, ...)
  errors/        programs that must be rejected with the expected errors
  run_tests.sh   runs all of the above (make test)
  fuzz.py        differential testing on random programs (make fuzz)
```

## Writing a test

A test is a single `.imp` file. Its first comment describes what the test checks and
lists the expectations, one per line:

```
( Every arithmetic operator on two numbers from the input.

  input:  7 3
  output: 10 4 21 2 1
  input:  -7 3
  output: -4 -10 -21 -3 2
)
VAR
    a, b, c
BEGIN
    ...
```

- `input:` – numbers given to `READ`, in order.
- `output:` – numbers expected from `WRITE`, in order. Each `output:` line is a separate
  run of the program, using the `input:` line above it (or no input). An empty `output:`
  means that the program prints nothing.
- `error: <line>: <message>` – in `tests/errors/` only: the compiler must fail and report
  this error. List several lines to require several errors. The message must match the
  beginning of the reported one, so `error: 6: syntax error, unexpected WRITE` accepts
  `syntax error, unexpected WRITE, expecting ;`.

The runner prints the execution cost of every run; the total at the end is a convenient
way to measure the effect of a change in code generation.

## Fuzzing

`fuzz.py` generates random programs that are valid and always terminate, runs each of them
with a reference interpreter written in Python and, after compilation, on the virtual
machine, and compares the printed numbers. Programs whose values could overflow 64 bits,
or that index arrays out of range, are skipped. A mismatching program is saved in
`build/fuzz/` as a test file with the input and the expected output in its first comment,
so it can be moved to `tests/programs/` once the bug is fixed.

```sh
make fuzz
python3 tests/fuzz.py --count 2000 --seed 100
```
