# Virtual machine

The target machine of the compiler, provided with the course as binaries for x86-64 Linux:

- `vm` – uses 64-bit integers,
- `vm-cln` – uses arbitrary-precision integers; requires the CLN library (`libcln.so.6`).

```sh
vm/vm program.mr
```

The machine reads the program, runs it, reads the input of `GET` from the standard input
and prints `PUT` results as `> value`. At the end it reports the cost of the run. Its
instructions and their costs are described in
[docs/architecture.md](../docs/architecture.md#the-virtual-machine).

`examples/` contains hand-written programs from the course materials, with comments in
Polish: `binary_digits.mr` (and an optimized version) prints the binary digits of a number,
`sieve.mr` is the sieve of Eratosthenes. They show what carefully written code for this
machine costs, which is a useful benchmark for the compiler.
