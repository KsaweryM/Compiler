#!/usr/bin/env bash
# Runs every test program in tests/programs and tests/errors.
#
# Each test states its expectations in its first comment (see tests/README.md):
#   input:  numbers for READ           one run per `input:` line, or a single run
#   output: numbers printed by WRITE   without input if the test has no `input:`
#   error:  <line>: <message>          tests/errors only; each one must be reported
#
# usage: tests/run_tests.sh [compiler] [virtual machine]

COMPILER=${1:-build/kompilator}
VM=${2:-vm/vm}
TESTS=$(cd "$(dirname "$0")" && pwd)

WORKDIR=$(mktemp -d) || exit 1
trap 'rm -rf "$WORKDIR"' EXIT

passed=0
failed=0
total_cost=0

pass() {
    passed=$((passed + 1))
    printf '  ok    %-44s %s\n' "$1" "$2"
}

fail() {
    failed=$((failed + 1))
    printf '  FAIL  %s\n' "$1"
    printf '        %s\n' "${@:2}"
}

# Prints the expectation lines (`input: ...`, `output: ...`, `error: ...`) of a test.
directives() {
    tr -d '\r' < "$1" | sed -n 's/^[[:space:]]*\(input\|output\|error\):[[:space:]]*/\1 /p'
}

# Runs compiled code on the virtual machine; prints the written numbers, then the cost.
# The machine reports in Polish: "Błąd" is an error, "koszt" the cost of the run.
execute() {
    local input=$1 output
    output=$(printf '%s\n' $input | "$VM" "$WORKDIR/program.mr" 2>&1 | sed 's/\x1b\[[0-9;]*m//g')

    if grep -q 'Błąd' <<< "$output"; then
        echo "virtual machine error: $(grep 'Błąd' <<< "$output")"
        return 1
    fi

    grep -o '> *-\?[0-9]\+' <<< "$output" | tr -d '> ' | tr '\n' ' '
    echo
    grep -o 'koszt: [0-9]\+' <<< "$output" | grep -o '[0-9]\+'
}

run_program_test() {
    local file=$1 name=${1#"$TESTS"/programs/}
    name=${name%.imp}

    if ! "$COMPILER" "$file" "$WORKDIR/program.mr" 2> "$WORKDIR/errors"; then
        fail "$name" "compilation failed:" "$(cat "$WORKDIR/errors")"
        return
    fi

    local input="" costs="" kind value result actual expected cost
    while read -r kind value; do
        if [ "$kind" == input ]; then
            input=$value
            continue
        fi

        if ! result=$(execute "$input"); then
            fail "$name" "input: $input" "$result"
            return
        fi

        actual=$(head -n 1 <<< "$result")
        actual=${actual% }
        cost=$(tail -n 1 <<< "$result")
        expected=$(echo $value)

        if [ "$actual" != "$expected" ]; then
            fail "$name" "input:    $input" "expected: $expected" "actual:   $actual"
            return
        fi

        costs+="${costs:+, }$cost"
        total_cost=$((total_cost + cost))
        input=""
    done < <(directives "$file")

    pass "$name" "cost $costs"
}

run_error_test() {
    local file=$1 name=${1#"$TESTS"/}
    name=${name%.imp}

    "$COMPILER" "$file" "$WORKDIR/program.mr" 2> "$WORKDIR/errors"
    local status=$?

    if [ $status -ne 1 ]; then
        fail "$name" "expected exit status 1, got $status" "$(cat "$WORKDIR/errors")"
        return
    fi

    local kind value line message
    while read -r kind value; do
        line=${value%%:*}
        message=${value#*: }
        if ! grep -qF ":$line: error: $message" "$WORKDIR/errors"; then
            fail "$name" "missing error on line $line: $message" "reported:" "$(cat "$WORKDIR/errors")"
            return
        fi
    done < <(directives "$file" | grep '^error ')

    pass "$name"
}

echo "programs"
while read -r file; do
    run_program_test "$file"
done < <(find "$TESTS/programs" -name '*.imp' | sort)

echo "errors"
while read -r file; do
    run_error_test "$file"
done < <(find "$TESTS/errors" -name '*.imp' | sort)

echo
echo "$passed passed, $failed failed (total cost of all program runs: $total_cost)"
[ "$failed" -eq 0 ]
