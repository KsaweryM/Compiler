#include "backend/CodeGenerator.h"

#include <climits>

#include "backend/Arithmetic.h"
#include "semantics/Symbol.h"

using namespace registers;
using Index = ast::Identifier::Index;

namespace {

unsigned long long magnitude(long long value) {
    unsigned long long bits = static_cast<unsigned long long>(value);
    return value < 0 ? 0 - bits : bits;
}

// Exponent k if |value| == 2^k, otherwise -1.
int powerOfTwoExponent(long long value) {
    unsigned long long m = magnitude(value);
    if (m == 0 || (m & (m - 1)) != 0) {
        return -1;
    }
    return __builtin_ctzll(m);
}

std::optional<long long> constantOf(const ast::Value& value) {
    if (value.isConstant) {
        return value.constant;
    }
    return std::nullopt;
}

// Address of a variable or of an array element with a constant index.
long long staticAddress(const ast::Identifier& identifier) {
    const Symbol& symbol = *identifier.symbol;
    return identifier.index == Index::CONSTANT ? symbol.addressOf(identifier.constantIndex) : symbol.address;
}

// Result of an operation on two constants, or nothing if it overflows.
std::optional<long long> fold(long long x, ast::Operator op, long long y) {
    long long result = 0;

    switch (op) {
    case ast::Operator::PLUS:
        return __builtin_add_overflow(x, y, &result) ? std::nullopt : std::optional(result);
    case ast::Operator::MINUS:
        return __builtin_sub_overflow(x, y, &result) ? std::nullopt : std::optional(result);
    case ast::Operator::TIMES:
        return __builtin_mul_overflow(x, y, &result) ? std::nullopt : std::optional(result);
    case ast::Operator::DIV:
        if (y == 0) {
            return 0;
        }
        if (x == LLONG_MIN && y == -1) {
            return std::nullopt;
        }
        result = x / y;
        return (x % y != 0 && (x < 0) != (y < 0)) ? result - 1 : result;
    case ast::Operator::MOD:
        if (y == 0 || y == -1) {
            return 0;
        }
        result = x % y;
        return (result != 0 && (result < 0) != (y < 0)) ? result + y : result;
    }

    return std::nullopt;
}

bool holds(long long x, ast::Relation relation, long long y) {
    switch (relation) {
    case ast::Relation::EQ: return x == y;
    case ast::Relation::NEQ: return x != y;
    case ast::Relation::LE: return x < y;
    case ast::Relation::GE: return x > y;
    case ast::Relation::LEQ: return x <= y;
    case ast::Relation::GEQ: return x >= y;
    }
    return false;
}

// Whether both values are read from the same memory cell, e.g. `x` and `x`, or `t[i]` and `t[i]`.
bool sameCell(const ast::Value& x, const ast::Value& y) {
    if (x.isConstant || y.isConstant) {
        return false;
    }

    const ast::Identifier& p = x.identifier;
    const ast::Identifier& q = y.identifier;

    if (p.symbol != q.symbol || p.index != q.index) {
        return false;
    }
    if (p.index == Index::CONSTANT) {
        return p.constantIndex == q.constantIndex;
    }
    return p.index == Index::NONE || p.indexSymbol == q.indexSymbol;
}

// The outcome of a condition known at compile time.
std::optional<bool> staticValue(const ast::Condition& condition) {
    auto x = constantOf(condition.left);
    auto y = constantOf(condition.right);

    if (x && y) {
        return holds(*x, condition.relation, *y);
    }
    if (sameCell(condition.left, condition.right)) {
        return holds(0, condition.relation, 0);
    }
    return std::nullopt;
}

// Relation after swapping the operands: x < y  <=>  y > x.
ast::Relation mirror(ast::Relation relation) {
    switch (relation) {
    case ast::Relation::LE: return ast::Relation::GE;
    case ast::Relation::GE: return ast::Relation::LE;
    case ast::Relation::LEQ: return ast::Relation::GEQ;
    case ast::Relation::GEQ: return ast::Relation::LEQ;
    default: return relation;
    }
}

ast::Relation negation(ast::Relation relation) {
    switch (relation) {
    case ast::Relation::EQ: return ast::Relation::NEQ;
    case ast::Relation::NEQ: return ast::Relation::EQ;
    case ast::Relation::LE: return ast::Relation::GEQ;
    case ast::Relation::GE: return ast::Relation::LEQ;
    case ast::Relation::LEQ: return ast::Relation::GE;
    case ast::Relation::GEQ: return ast::Relation::LE;
    }
    return relation;
}

}

Code CodeGenerator::generate(const ast::Program& program) {
    code_ = Code();
    generate(program.commands);
    code_.halt();
    return std::move(code_);
}

void CodeGenerator::generate(const ast::CommandList& commands) {
    for (const ast::Command& command : commands) {
        std::visit([this](const auto& node) { generate(node); }, command.node);
    }
}

void CodeGenerator::generate(const ast::Assign& assign) {
    evaluate(assign.expression);
    store(assign.target);
}

void CodeGenerator::generate(const ast::If& statement) {
    if (auto value = staticValue(statement.condition)) {
        generate(*value ? statement.thenBranch : statement.elseBranch);
        return;
    }

    Label elseBranch = code_.newLabel();
    branch(statement.condition, elseBranch, false);
    generate(statement.thenBranch);

    if (statement.elseBranch.empty()) {
        code_.bind(elseBranch);
        return;
    }

    Label end = code_.newLabel();
    code_.jump(end);
    code_.bind(elseBranch);
    generate(statement.elseBranch);
    code_.bind(end);
}

// The condition is placed after the body, so each iteration costs one jump less:
//     JUMP check; body: ...; check: if condition then JUMP body
void CodeGenerator::generate(const ast::While& loop) {
    auto value = staticValue(loop.condition);
    if (value == false) {
        return;
    }

    Label body = code_.newLabel(), check = code_.newLabel();

    if (!value) {
        code_.jump(check);
    }

    code_.bind(body);
    generate(loop.body);

    if (value) {
        code_.jump(body);
        return;
    }

    code_.bind(check);
    branch(loop.condition, body, true);
}

void CodeGenerator::generate(const ast::Repeat& loop) {
    Label body = code_.newLabel();

    code_.bind(body);
    generate(loop.body);
    branch(loop.condition, body, false);
}

// The final value is computed once, before the first iteration. A constant final value is
// built into the comparison; any other is kept in a hidden memory cell.
void CodeGenerator::generate(const ast::For& loop) {
    const long long iterator = loop.iterator->address;
    const auto finalConstant = constantOf(loop.to);

    bool boundInCode = false;
    if (finalConstant && *finalConstant != LLONG_MIN) {
        long long inCode = addConstantCost(-*finalConstant);
        long long inMemory = cost(Opcode::SWAP) + Code::setConstantCost(A, loop.bound->address, true) +
                             cost(Opcode::LOAD) + cost(Opcode::SUB);
        boundInCode = inCode <= inMemory;
    }

    if (!boundInCode) {
        loadValue(loop.to);
        code_.setConstantPreservingA(H, loop.bound->address, B);
        code_.store(H);
    }

    loadValue(loop.from);
    code_.setConstantPreservingA(H, iterator, B);
    code_.store(H);

    Label body = code_.newLabel(), check = code_.newLabel();
    code_.jump(check);

    code_.bind(body);
    generate(loop.body);
    code_.setConstant(H, iterator, H);
    code_.load(H);
    if (loop.descending) {
        code_.dec(A);
    }
    else {
        code_.inc(A);
    }
    code_.store(H);

    // a = iterator
    code_.bind(check);
    if (boundInCode) {
        addConstant(-*finalConstant);           // a = iterator - final
        if (loop.descending) {
            code_.jpos(body);
        }
        else {
            code_.jneg(body);
        }
    }
    else {
        code_.swap(C);
        code_.setConstant(A, loop.bound->address, B);
        code_.load(A);
        code_.sub(C);                           // a = final - iterator
        if (loop.descending) {
            code_.jneg(body);
        }
        else {
            code_.jpos(body);
        }
    }
    code_.jzero(body);
}

void CodeGenerator::generate(const ast::Read& read) {
    if (read.target.index == Index::VARIABLE) {
        loadAddress(read.target, H);
    }
    else {
        code_.setConstant(H, staticAddress(read.target), H);
    }

    code_.get();
    code_.store(H);
}

void CodeGenerator::generate(const ast::Write& write) {
    loadValue(write.value);
    code_.put();
}

// a = expression. Clobbers b-g.
void CodeGenerator::evaluate(const ast::Expression& expression) {
    const ast::Value& left = expression.left;
    const ast::Value& right = expression.right;

    if (!expression.op) {
        loadValue(left);
        return;
    }

    const ast::Operator op = *expression.op;
    const auto x = constantOf(left);
    const auto y = constantOf(right);

    if (x && y) {
        if (auto result = fold(*x, op, *y)) {
            code_.setConstant(A, *result, B);
            return;
        }
    }

    if (sameCell(left, right)) {
        switch (op) {
        case ast::Operator::PLUS:                // x + x = 2x
            loadValue(left);
            code_.add(A);
            return;
        case ast::Operator::MINUS:               // x - x = 0
        case ast::Operator::MOD:                 // x mod x = 0, also for x = 0
            code_.reset(A);
            return;
        case ast::Operator::DIV: {               // x / x = 1, or 0 for x = 0
            Label zero = code_.newLabel();
            loadValue(left);
            code_.jzero(zero);
            code_.reset(A);
            code_.inc(A);
            code_.bind(zero);
            return;
        }
        case ast::Operator::TIMES:
            break;
        }
    }

    switch (op) {
    case ast::Operator::PLUS:
        if (y) {
            loadValue(left);
            addConstant(*y);
        }
        else if (x) {
            loadValue(right);
            addConstant(*x);
        }
        else {
            loadOperands(left, right);
            code_.add(C);
        }
        return;

    case ast::Operator::MINUS:
        if (y) {
            loadValue(left);
            subtractConstant(*y);
        }
        else {
            loadOperands(left, right);
            code_.sub(C);
        }
        return;

    case ast::Operator::TIMES:
        if (y || x) {
            loadValue(y ? left : right);
            arithmetic::multiplyByConstant(code_, y ? *y : *x);
        }
        else {
            loadOperands(left, right);
            arithmetic::multiply(code_);
        }
        return;

    case ast::Operator::DIV:
        if (y && (*y == 0 || *y == 1 || *y == -1 || powerOfTwoExponent(*y) > 0)) {
            if (*y == 0) {
                code_.reset(A);
                return;
            }
            loadValue(left);
            if (*y < 0) {
                arithmetic::negate(code_);      // x / -2^k == -x / 2^k
            }
            if (powerOfTwoExponent(*y) > 0) {
                arithmetic::divideByPowerOfTwo(code_, powerOfTwoExponent(*y));
            }
            return;
        }
        loadOperands(left, right);
        arithmetic::divide(code_);
        return;

    case ast::Operator::MOD:
        if (y && (*y == 0 || *y == 1 || *y == -1)) {
            code_.reset(A);
            return;
        }
        if (y && *y > 0 && powerOfTwoExponent(*y) > 0) {
            loadValue(left);
            arithmetic::moduloByPowerOfTwo(code_, powerOfTwoExponent(*y));
            return;
        }
        loadOperands(left, right);
        arithmetic::modulo(code_);
        return;
    }
}

// a = left, c = right. Clobbers b.
void CodeGenerator::loadOperands(const ast::Value& left, const ast::Value& right) {
    if (sameCell(left, right)) {
        loadValue(left);
        code_.swap(C);
        code_.reset(A);
        code_.add(C);
        return;
    }

    if (auto y = constantOf(right)) {
        code_.setConstant(C, *y, C);
    }
    else {
        loadValue(right);
        code_.swap(C);
    }

    loadValue(left);
}

// a = value. Clobbers b.
void CodeGenerator::loadValue(const ast::Value& value) {
    if (value.isConstant) {
        code_.setConstant(A, value.constant, B);
    }
    else {
        loadIdentifier(value.identifier);
    }
}

void CodeGenerator::loadIdentifier(const ast::Identifier& identifier) {
    if (identifier.index == Index::VARIABLE) {
        loadIndexedAddressIntoA(identifier, B);
    }
    else {
        code_.setConstant(A, staticAddress(identifier), B);
    }

    code_.load(A);
}

// target = address of the identifier. `target` must not be a. Clobbers a.
void CodeGenerator::loadAddress(const ast::Identifier& identifier, Register target) {
    if (identifier.index == Index::VARIABLE) {
        loadIndexedAddressIntoA(identifier, target);
        code_.swap(target);
    }
    else {
        code_.setConstant(target, staticAddress(identifier), target);
    }
}

// a = address of t[i] = i + (address of t - first index of t). Clobbers `temporary`.
void CodeGenerator::loadIndexedAddressIntoA(const ast::Identifier& identifier, Register temporary) {
    const Symbol& array = *identifier.symbol;
    const long long offset = array.address - array.firstIndex;

    const unsigned long long m = magnitude(offset);
    const long long viaRegister = Code::setConstantCost(temporary, offset, true) + cost(Opcode::ADD);
    const bool byIncrements = m <= static_cast<unsigned long long>(viaRegister);

    if (!byIncrements) {
        code_.setConstant(temporary, offset, temporary);
    }

    code_.setConstant(A, identifier.indexSymbol->address,
                      byIncrements ? std::optional(temporary) : std::nullopt);
    code_.load(A);

    if (!byIncrements) {
        code_.add(temporary);
    }
    else if (offset < 0) {
        code_.dec(A, m);
    }
    else {
        code_.inc(A, m);
    }
}

// Stores register a in the target. Clobbers b, h.
void CodeGenerator::store(const ast::Identifier& target) {
    if (target.index == Index::VARIABLE) {
        code_.swap(H);                          // park the value in h
        loadIndexedAddressIntoA(target, B);
        code_.swap(H);
    }
    else {
        code_.setConstantPreservingA(H, staticAddress(target), B);
    }

    code_.store(H);
}

// a += value, either by repeated INC/DEC or by building the value in c. Clobbers b, c.
void CodeGenerator::addConstant(long long value) {
    if (addByIncrements(value)) {
        if (value < 0) {
            code_.dec(A, magnitude(value));
        }
        else {
            code_.inc(A, magnitude(value));
        }
        return;
    }

    code_.swap(C);
    code_.setConstant(A, value, B);
    code_.add(C);
}

// a -= value. Clobbers b, c.
void CodeGenerator::subtractConstant(long long value) {
    if (value != LLONG_MIN) {
        addConstant(-value);
        return;
    }

    code_.swap(C);
    code_.setConstant(A, value, B);
    code_.swap(C);
    code_.sub(C);
}

long long CodeGenerator::addConstantCost(long long value) const {
    if (addByIncrements(value)) {
        return static_cast<long long>(magnitude(value)) * cost(Opcode::INC);
    }
    return addViaRegisterCost(value);
}

bool CodeGenerator::addByIncrements(long long value) const {
    return magnitude(value) <= static_cast<unsigned long long>(addViaRegisterCost(value) / cost(Opcode::INC));
}

long long CodeGenerator::addViaRegisterCost(long long value) const {
    return cost(Opcode::SWAP) + Code::setConstantCost(A, value, true) + cost(Opcode::ADD);
}

// Jumps to `target` if the condition is true (jumpIfTrue) or false (!jumpIfTrue).
// Clobbers a, b, c.
void CodeGenerator::branch(const ast::Condition& condition, Label target, bool jumpIfTrue) {
    if (auto value = staticValue(condition)) {
        if (*value == jumpIfTrue) {
            code_.jump(target);
        }
        return;
    }

    const ast::Value* left = &condition.left;
    const ast::Value* right = &condition.right;
    ast::Relation relation = condition.relation;

    if (left->isConstant) {
        std::swap(left, right);
        relation = mirror(relation);
    }

    // a = left - right
    if (auto y = constantOf(*right)) {
        loadValue(*left);
        subtractConstant(*y);
    }
    else {
        loadOperands(*left, *right);
        code_.sub(C);
    }

    if (!jumpIfTrue) {
        relation = negation(relation);
    }

    switch (relation) {
    case ast::Relation::EQ:
        code_.jzero(target);
        break;
    case ast::Relation::NEQ:
        code_.jpos(target);
        code_.jneg(target);
        break;
    case ast::Relation::LE:
        code_.jneg(target);
        break;
    case ast::Relation::GE:
        code_.jpos(target);
        break;
    case ast::Relation::LEQ:
        code_.jneg(target);
        code_.jzero(target);
        break;
    case ast::Relation::GEQ:
        code_.jpos(target);
        code_.jzero(target);
        break;
    }
}
