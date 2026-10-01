#!/usr/bin/env python3
"""Differential testing of the compiler.

Generates random (but valid and terminating) programs, runs each of them with a reference
interpreter written in Python and, after compilation, on the virtual machine, and compares
the printed numbers. A mismatching program is saved so that it can be reproduced:

    python3 tests/fuzz.py --compiler build/kompilator --vm vm/vm --count 500 --seed 7
"""

import argparse
import os
import random
import re
import subprocess
import sys
import tempfile

# The virtual machine uses 64-bit integers; programs whose values leave this range are skipped
# so that intermediate results of multiplication and division cannot overflow.
VALUE_LIMIT = 2 ** 40
MAX_STEPS = 20_000

OPERATORS = ["PLUS", "MINUS", "TIMES", "DIV", "MOD"]
RELATIONS = ["EQ", "NEQ", "LE", "GE", "LEQ", "GEQ"]


class Skip(Exception):
    """The generated program is not usable (too slow, overflows, index out of range...)."""


# --- program representation ---------------------------------------------------------------
#
# identifier: ("var", name) | ("elem", array, index) | ("elem_var", array, index_name)
# value:      ("num", n) | identifier
# expression: (value,) | (value, operator, value)
# condition:  (value, relation, value)
# command:    ("assign", identifier, expression) | ("read", identifier) | ("write", value)
#             ("if", condition, commands, commands) | ("while", condition, commands)
#             ("repeat", commands, condition) | ("for", name, value, value, descending, commands)


def render_value(value):
    kind = value[0]
    if kind == "num":
        return str(value[1])
    if kind == "var":
        return value[1]
    return f"{value[1]}[{value[2]}]"


def render_expression(expression):
    if len(expression) == 1:
        return render_value(expression[0])
    return f"{render_value(expression[0])} {expression[1]} {render_value(expression[2])}"


def render_condition(condition):
    return f"{render_value(condition[0])} {condition[1]} {render_value(condition[2])}"


def render_commands(commands, indent):
    lines = []
    pad = "    " * indent
    for command in commands:
        kind = command[0]
        if kind == "assign":
            lines.append(f"{pad}{render_value(command[1])} ASSIGN {render_expression(command[2])};")
        elif kind == "read":
            lines.append(f"{pad}READ {render_value(command[1])};")
        elif kind == "write":
            lines.append(f"{pad}WRITE {render_value(command[1])};")
        elif kind == "if":
            lines.append(f"{pad}IF {render_condition(command[1])} THEN")
            lines += render_commands(command[2], indent + 1)
            if command[3]:
                lines.append(f"{pad}ELSE")
                lines += render_commands(command[3], indent + 1)
            lines.append(f"{pad}ENDIF")
        elif kind == "while":
            lines.append(f"{pad}WHILE {render_condition(command[1])} DO")
            lines += render_commands(command[2], indent + 1)
            lines.append(f"{pad}ENDWHILE")
        elif kind == "repeat":
            lines.append(f"{pad}REPEAT")
            lines += render_commands(command[1], indent + 1)
            lines.append(f"{pad}UNTIL {render_condition(command[2])};")
        elif kind == "for":
            _, name, start, end, descending, body = command
            direction = "DOWNTO" if descending else "TO"
            lines.append(f"{pad}FOR {name} FROM {render_value(start)} {direction} {render_value(end)} DO")
            lines += render_commands(body, indent + 1)
            lines.append(f"{pad}ENDFOR")
    return lines


def render_program(scalars, arrays, commands, header):
    declarations = list(scalars) + [f"{name}[{lo}:{hi}]" for name, (lo, hi) in arrays.items()]
    return "\n".join(
        [f"( {header} )", "VAR", "    " + ", ".join(declarations), "BEGIN"]
        + render_commands(commands, 1)
        + ["END", ""]
    )


# --- reference interpreter -----------------------------------------------------------------


class Interpreter:
    def __init__(self, scalars, arrays, inputs):
        self.scopes = [{name: [0] for name in scalars}]
        self.arrays = {name: (lo, hi, {}) for name, (lo, hi) in arrays.items()}
        self.inputs = list(inputs)
        self.outputs = []
        self.steps = 0

    def cell(self, name):
        for scope in reversed(self.scopes):
            if name in scope:
                return scope[name]
        raise AssertionError(f"undeclared {name}")

    def element(self, identifier):
        lo, hi, values = self.arrays[identifier[1]]
        index = identifier[2] if identifier[0] == "elem" else self.cell(identifier[2])[0]
        if not lo <= index <= hi:
            raise Skip("index out of range")
        return values, index

    def get(self, value):
        if value[0] == "num":
            return value[1]
        if value[0] == "var":
            return self.cell(value[1])[0]
        values, index = self.element(value)
        return values.get(index, 0)

    def set(self, identifier, number):
        if abs(number) >= VALUE_LIMIT:
            raise Skip("value too large")
        if identifier[0] == "var":
            self.cell(identifier[1])[0] = number
        else:
            values, index = self.element(identifier)
            values[index] = number

    def evaluate(self, expression):
        x = self.get(expression[0])
        if len(expression) == 1:
            return x
        op, y = expression[1], self.get(expression[2])
        if op == "PLUS":
            result = x + y
        elif op == "MINUS":
            result = x - y
        elif op == "TIMES":
            result = x * y
        elif op == "DIV":
            result = 0 if y == 0 else x // y
        else:
            result = 0 if y == 0 else x % y
        if abs(result) >= VALUE_LIMIT:
            raise Skip("value too large")
        return result

    def holds(self, condition):
        x, relation, y = self.get(condition[0]), condition[1], self.get(condition[2])
        return {
            "EQ": x == y, "NEQ": x != y, "LE": x < y, "GE": x > y, "LEQ": x <= y, "GEQ": x >= y,
        }[relation]

    def run(self, commands):
        for command in commands:
            self.steps += 1
            if self.steps > MAX_STEPS:
                raise Skip("too many steps")

            kind = command[0]
            if kind == "assign":
                self.set(command[1], self.evaluate(command[2]))
            elif kind == "read":
                if not self.inputs:
                    raise Skip("out of input")
                self.set(command[1], self.inputs.pop(0))
            elif kind == "write":
                self.outputs.append(self.get(command[1]))
            elif kind == "if":
                self.run(command[2] if self.holds(command[1]) else command[3])
            elif kind == "while":
                while self.holds(command[1]):
                    self.run(command[2])
            elif kind == "repeat":
                while True:
                    self.run(command[1])
                    if self.holds(command[2]):
                        break
            elif kind == "for":
                _, name, start, end, descending, body = command
                first, last = self.get(start), self.get(end)
                values = range(first, last - 1, -1) if descending else range(first, last + 1)
                iterator = [first]
                self.scopes.append({name: iterator})
                for value in values:
                    iterator[0] = value
                    self.run(body)
                    self.steps += 1
                    if self.steps > MAX_STEPS:
                        raise Skip("too many steps")
                self.scopes.pop()


# --- random program generator --------------------------------------------------------------


class Generator:
    def __init__(self, rng):
        self.rng = rng
        self.scalars = [f"x{chr(ord('a') + i)}" for i in range(rng.randint(2, 6))]
        self.arrays = {}
        for i in range(rng.randint(0, 3)):
            lo = rng.choice([0, 1, -5, rng.randint(-50, 50), rng.randint(-10 ** 9, 10 ** 9)])
            self.arrays[f"t{chr(ord('a') + i)}"] = (lo, lo + rng.randint(0, 12))
        self.guards = []
        self.initialized = set()
        # innermost last: name -> ("scalar",) | ("iterator", lo, hi or None)
        self.scopes = [{name: ("scalar",) for name in self.scalars}]
        self.iterator_counter = 0

    # names visible right now
    def binding(self, name):
        for scope in reversed(self.scopes):
            if name in scope:
                return scope[name]
        return None

    def readable_scalars(self):
        names = set()
        for scope in self.scopes:
            names.update(scope)
        return [n for n in sorted(names)
                if (self.binding(n)[0] == "iterator") or (n in self.initialized)]

    def writable_scalars(self):
        return [n for n in self.scalars if self.binding(n) == ("scalar",)]

    def constant(self):
        r = self.rng.random()
        if r < 0.45:
            return self.rng.randint(-10, 10)
        if r < 0.7:
            return self.rng.choice([-1, 1]) * self.rng.randint(11, 1000)
        if r < 0.9:
            return self.rng.choice([-1, 1]) * 2 ** self.rng.randint(1, 20)
        return self.rng.choice([-1, 1]) * self.rng.randint(10 ** 5, 10 ** 9)

    def element(self, readable):
        name = self.rng.choice(sorted(self.arrays))
        lo, hi = self.arrays[name]
        safe_indices = [n for n in readable
                        if self.binding(n)[0] == "iterator" and self.binding(n)[1] is not None
                        and lo <= self.binding(n)[1] and self.binding(n)[2] <= hi]
        if safe_indices and self.rng.random() < 0.6:
            return ("elem_var", name, self.rng.choice(safe_indices))
        if readable and self.rng.random() < 0.1:
            return ("elem_var", name, self.rng.choice(readable))   # often out of range -> skipped
        return ("elem", name, self.rng.randint(lo, hi))

    def value(self):
        readable = self.readable_scalars()
        r = self.rng.random()
        if r < 0.35 or (not readable and not self.arrays):
            return ("num", self.constant())
        if self.arrays and (r > 0.8 or not readable):
            return self.element(readable)
        return ("var", self.rng.choice(readable))

    def target(self):
        writable = self.writable_scalars()
        if self.arrays and (not writable or self.rng.random() < 0.35):
            return self.element(self.readable_scalars())
        return ("var", self.rng.choice(writable))

    def expression(self):
        if self.rng.random() < 0.2:
            return (self.value(),)
        return (self.value(), self.rng.choice(OPERATORS), self.value())

    def condition(self):
        return (self.value(), self.rng.choice(RELATIONS), self.value())

    def mark(self, target):
        if target[0] == "var":
            self.initialized.add(target[1])

    def new_guard(self):
        name = f"g{chr(ord('a') + len(self.guards))}"
        self.guards.append(name)
        self.scopes[0][name] = ("guard",)
        return name

    def commands(self, depth):
        return [self.command(depth) for _ in range(self.rng.randint(1, 4))]

    def command(self, depth):
        kinds = ["assign"] * 5 + ["write"] * 2 + ["read"]
        if depth < 3:
            kinds += ["if", "if", "while", "repeat", "for", "for"]
        kind = self.rng.choice(kinds)

        # every variable may be hidden by a loop iterator, leaving nothing to assign to
        if kind in ("assign", "read") and not self.writable_scalars() and not self.arrays:
            kind = "write"

        if kind == "assign":
            target, expression = self.target(), self.expression()
            self.mark(target)
            return ("assign", target, expression)
        if kind == "read":
            target = self.target()
            self.mark(target)
            return ("read", target)
        if kind == "write":
            return ("write", self.value())
        if kind == "if":
            # generated in source order, so that initialization is tracked like in the compiler
            condition = self.condition()
            then_branch = self.commands(depth + 1)
            else_branch = self.commands(depth + 1) if self.rng.random() < 0.5 else []
            return ("if", condition, then_branch, else_branch)
        if kind == "while":
            return self.guarded_while(depth)
        if kind == "repeat":
            return self.guarded_repeat(depth)
        return self.for_loop(depth)

    # WHILE and REPEAT loops count with a dedicated guard variable, so they always terminate.
    def guarded_while(self, depth):
        guard, limit = self.new_guard(), self.rng.randint(0, 4)
        condition = self.rng.choice([
            (("var", guard), "LE", ("num", limit)),
            (("var", guard), "LEQ", ("num", limit - 1)),
            (("num", limit), "GE", ("var", guard)),
            (("var", guard), "NEQ", ("num", limit)),
        ])
        self.initialized.add(guard)
        body = self.commands(depth + 1)
        step = ("assign", ("var", guard), (("var", guard), "PLUS", ("num", 1)))
        return ("block", [("assign", ("var", guard), (("num", 0),)), ("while", condition, body + [step])])

    def guarded_repeat(self, depth):
        guard, limit = self.new_guard(), self.rng.randint(1, 4)
        self.initialized.add(guard)
        body = self.commands(depth + 1)
        step = ("assign", ("var", guard), (("var", guard), "PLUS", ("num", 1)))
        condition = self.rng.choice([
            (("var", guard), "GEQ", ("num", limit)),
            (("var", guard), "EQ", ("num", limit)),
            (("num", limit), "LEQ", ("var", guard)),
        ])
        return ("block", [("assign", ("var", guard), (("num", 0),)), ("repeat", body + [step], condition)])

    def for_loop(self, depth):
        # Iterators sometimes reuse the name of a variable or of an outer iterator (shadowing).
        r = self.rng.random()
        outer_iterators = [n for scope in self.scopes[1:] for n in scope]
        if r < 0.15:
            name = self.rng.choice(self.scalars)
        elif r < 0.25 and outer_iterators:
            name = self.rng.choice(outer_iterators)
        else:
            self.iterator_counter += 1
            name = "i" * self.iterator_counter

        descending = self.rng.random() < 0.4
        if self.rng.random() < 0.7:
            lo = self.rng.choice([0, 1, self.rng.randint(-20, 20)])
            if self.arrays and self.rng.random() < 0.5:
                array_lo, array_hi = self.arrays[self.rng.choice(sorted(self.arrays))]
                lo = self.rng.randint(array_lo, array_hi)
                hi = self.rng.randint(lo, array_hi)
            else:
                hi = lo + self.rng.randint(-1, 6)
            start, end = (hi, lo) if descending else (lo, hi)
            start_value, end_value = ("num", start), ("num", end)
            known = (lo, hi)
        else:
            start_value, end_value = self.value(), self.value()
            known = (None, None)

        self.scopes.append({name: ("iterator",) + known})
        body = self.commands(depth + 1)
        self.scopes.pop()
        return ("for", name, start_value, end_value, descending, body)

    def program(self):
        commands = self.commands(0)
        commands += [("write", ("var", n)) for n in self.scalars if n in self.initialized]
        return flatten(commands)


def flatten(commands):
    """Expands the ("block", [...]) helper nodes produced for guarded loops."""
    result = []
    for command in commands:
        if command[0] == "block":
            result += flatten(command[1])
        elif command[0] == "if":
            result.append(("if", command[1], flatten(command[2]), flatten(command[3])))
        elif command[0] == "while":
            result.append(("while", command[1], flatten(command[2])))
        elif command[0] == "repeat":
            result.append(("repeat", flatten(command[1]), command[2]))
        elif command[0] == "for":
            result.append(command[:5] + (flatten(command[5]),))
        else:
            result.append(command)
    return result


# --- driver ---------------------------------------------------------------------------------


def run_on_vm(compiler, vm, source, inputs, workdir):
    source_path = os.path.join(workdir, "program.imp")
    code_path = os.path.join(workdir, "program.mr")
    with open(source_path, "w") as file:
        file.write(source)

    compilation = subprocess.run([compiler, source_path, code_path], capture_output=True, text=True)
    if compilation.returncode != 0:
        return None, "compilation failed:\n" + compilation.stderr

    stdin = "".join(f"{number}\n" for number in inputs)
    execution = subprocess.run([vm, code_path], input=stdin, capture_output=True, text=True, timeout=60)
    output = re.sub(r"\x1b\[[0-9;]*m", "", execution.stdout + execution.stderr)
    if "Błąd" in output:    # the virtual machine reports errors in Polish ("Błąd" = "Error")
        return None, "virtual machine error:\n" + output
    return [int(n) for n in re.findall(r">\s*(-?\d+)", output)], None


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--compiler", default="build/kompilator")
    parser.add_argument("--vm", default="vm/vm")
    parser.add_argument("--count", type=int, default=300, help="number of programs to check")
    parser.add_argument("--seed", type=int, default=1, help="seed of the first program")
    parser.add_argument("--save-dir", default="build/fuzz", help="where failing programs are written")
    args = parser.parse_args()

    checked = skipped = failures = 0
    seed = args.seed

    with tempfile.TemporaryDirectory() as workdir:
        while checked < args.count:
            rng = random.Random(seed)
            generator = Generator(rng)
            commands = generator.program()
            inputs = [rng.choice([rng.randint(-20, 20), rng.randint(-10 ** 6, 10 ** 6)]) for _ in range(30)]

            interpreter = Interpreter(generator.scalars + generator.guards, generator.arrays, inputs)
            try:
                interpreter.run(commands)
            except Skip:
                skipped += 1
                seed += 1
                continue

            variables = generator.scalars + generator.guards
            source = render_program(variables, generator.arrays, commands, f"random program, seed {seed}")
            actual, problem = run_on_vm(args.compiler, args.vm, source, inputs, workdir)
            expected = interpreter.outputs

            if problem or actual != expected:
                failures += 1
                os.makedirs(args.save_dir, exist_ok=True)
                path = os.path.join(args.save_dir, f"failure-{seed}.imp")
                # saved in the format of tests/programs, ready to be added to the test suite
                header = (f"Random program found by fuzz.py, seed {seed}.\n\n"
                          f"  input:  {' '.join(map(str, inputs))}\n"
                          f"  output: {' '.join(map(str, expected))}\n")
                with open(path, "w") as file:
                    file.write(render_program(variables, generator.arrays, commands, header))
                print(f"MISMATCH (seed {seed}), program saved to {path}")
                print(f"  input:    {' '.join(map(str, inputs))}")
                print(f"  expected: {' '.join(map(str, expected))}")
                print(f"  actual:   {problem if problem else ' '.join(map(str, actual))}")

            checked += 1
            seed += 1

    print(f"checked {checked} programs ({skipped} generated programs skipped), {failures} mismatches")
    sys.exit(1 if failures else 0)


if __name__ == "__main__":
    main()
