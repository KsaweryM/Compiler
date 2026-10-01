#include "backend/Code.h"

#include <climits>
#include <stdexcept>

using namespace registers;

namespace {

const char* name(Opcode opcode) {
    switch (opcode) {
    case Opcode::GET: return "GET";
    case Opcode::PUT: return "PUT";
    case Opcode::LOAD: return "LOAD";
    case Opcode::STORE: return "STORE";
    case Opcode::ADD: return "ADD";
    case Opcode::SUB: return "SUB";
    case Opcode::SHIFT: return "SHIFT";
    case Opcode::SWAP: return "SWAP";
    case Opcode::RESET: return "RESET";
    case Opcode::INC: return "INC";
    case Opcode::DEC: return "DEC";
    case Opcode::JUMP: return "JUMP";
    case Opcode::JPOS: return "JPOS";
    case Opcode::JZERO: return "JZERO";
    case Opcode::JNEG: return "JNEG";
    case Opcode::HALT: return "HALT";
    }
    throw std::logic_error("unknown opcode");
}

bool isJump(Opcode opcode) {
    return opcode == Opcode::JUMP || opcode == Opcode::JPOS || opcode == Opcode::JZERO || opcode == Opcode::JNEG;
}

bool hasRegisterOperand(Opcode opcode) {
    return !isJump(opcode) && opcode != Opcode::GET && opcode != Opcode::PUT && opcode != Opcode::HALT;
}

unsigned long long magnitude(long long value) {
    unsigned long long bits = static_cast<unsigned long long>(value);
    return value < 0 ? 0 - bits : bits;
}

int bitLength(unsigned long long value) {
    return value == 0 ? 0 : 64 - __builtin_clzll(value);
}

// How to build a constant. Every method starts from RESET and repeats INC (or DEC, for
// negative numbers) to reach the value of the leading bits; the remaining `lowBits` bits
// are appended one by one: double the value (ADD a, or SHIFT by a register holding 1),
// then INC/DEC if the bit is set.
struct ConstantPlan {
    enum class Method { LINEAR, DOUBLE_BY_ADD, DOUBLE_BY_SHIFT };

    Method method = Method::LINEAR;
    int lowBits = 0;
    long long cost = LLONG_MAX;
};

ConstantPlan planConstant(Register target, long long value, bool hasScratch) {
    const unsigned long long limit = LLONG_MAX / 4;
    const unsigned long long m = magnitude(value);

    ConstantPlan best;
    if (m < limit) {
        best = {ConstantPlan::Method::LINEAR, 0, cost(Opcode::RESET) + static_cast<long long>(m) * cost(Opcode::INC)};
    }

    const long long swapCost = target == A ? 0 : cost(Opcode::SWAP);

    for (int lowBits = 1; lowBits < bitLength(m); lowBits++) {
        unsigned long long prefix = m >> lowBits;
        if (prefix >= limit) {
            continue;
        }

        long long ones = __builtin_popcountll(m & ((1ULL << lowBits) - 1));
        long long common = cost(Opcode::RESET) + static_cast<long long>(prefix + ones) * cost(Opcode::INC) + swapCost;

        long long byAdd = common + lowBits * cost(Opcode::ADD);
        if (byAdd < best.cost) {
            best = {ConstantPlan::Method::DOUBLE_BY_ADD, lowBits, byAdd};
        }

        if (hasScratch) {
            long long byShift = common + cost(Opcode::RESET) + cost(Opcode::INC) + lowBits * cost(Opcode::SHIFT);
            if (byShift < best.cost) {
                best = {ConstantPlan::Method::DOUBLE_BY_SHIFT, lowBits, byShift};
            }
        }
    }

    return best;
}

}

long long cost(Opcode opcode) {
    switch (opcode) {
    case Opcode::GET:
    case Opcode::PUT:
        return 100;
    case Opcode::LOAD:
    case Opcode::STORE:
        return 50;
    case Opcode::ADD:
    case Opcode::SUB:
        return 10;
    case Opcode::SHIFT:
        return 5;
    case Opcode::HALT:
        return 0;
    default:
        return 1;
    }
}

Label Code::newLabel() {
    labelPositions_.emplace_back();
    return Label{labelPositions_.size() - 1};
}

void Code::bind(Label label) {
    if (labelPositions_.at(label.id)) {
        throw std::logic_error("label bound twice");
    }
    labelPositions_[label.id] = instructions_.size();
}

void Code::get() { emit(Opcode::GET); }
void Code::put() { emit(Opcode::PUT); }
void Code::load(Register r) { emit(Opcode::LOAD, r); }
void Code::store(Register r) { emit(Opcode::STORE, r); }
void Code::add(Register r) { emit(Opcode::ADD, r); }
void Code::sub(Register r) { emit(Opcode::SUB, r); }
void Code::shift(Register r) { emit(Opcode::SHIFT, r); }
void Code::swap(Register r) { emit(Opcode::SWAP, r); }
void Code::reset(Register r) { emit(Opcode::RESET, r); }
void Code::inc(Register r) { emit(Opcode::INC, r); }
void Code::dec(Register r) { emit(Opcode::DEC, r); }
void Code::jump(Label target) { emitJump(Opcode::JUMP, target); }
void Code::jpos(Label target) { emitJump(Opcode::JPOS, target); }
void Code::jzero(Label target) { emitJump(Opcode::JZERO, target); }
void Code::jneg(Label target) { emitJump(Opcode::JNEG, target); }
void Code::halt() { emit(Opcode::HALT); }

void Code::inc(Register r, unsigned long long times) {
    for (unsigned long long i = 0; i < times; i++) {
        inc(r);
    }
}

void Code::dec(Register r, unsigned long long times) {
    for (unsigned long long i = 0; i < times; i++) {
        dec(r);
    }
}

void Code::setConstant(Register target, long long value, std::optional<Register> scratch) {
    if (scratch == A) {
        scratch.reset();
    }

    const ConstantPlan plan = planConstant(target, value, scratch.has_value());
    const unsigned long long m = magnitude(value);
    const Register one = scratch.value_or(A);   // holds 1 when doubling by SHIFT

    auto addOne = [&](Register r) {
        if (value < 0) {
            dec(r);
        }
        else {
            inc(r);
        }
    };

    if (plan.method == ConstantPlan::Method::LINEAR) {
        reset(target);
        for (unsigned long long i = 0; i < m; i++) {
            addOne(target);
        }
        return;
    }

    if (plan.method == ConstantPlan::Method::DOUBLE_BY_SHIFT) {
        reset(one);
        inc(one);
    }

    reset(A);
    for (unsigned long long i = 0; i < (m >> plan.lowBits); i++) {
        addOne(A);
    }

    for (int bit = plan.lowBits - 1; bit >= 0; bit--) {
        if (plan.method == ConstantPlan::Method::DOUBLE_BY_SHIFT) {
            shift(one);
        }
        else {
            add(A);
        }

        if ((m >> bit) & 1) {
            addOne(A);
        }
    }

    if (target != A) {
        swap(target);
    }
}

void Code::setConstantPreservingA(Register target, long long value, std::optional<Register> scratch) {
    // Either INC/DEC the target directly, or park a in the target while a builds the value.
    const unsigned long long m = magnitude(value);
    const long long viaA = 2 * cost(Opcode::SWAP) + planConstant(A, value, scratch.has_value()).cost;

    if (m < LLONG_MAX / 4 && cost(Opcode::RESET) + static_cast<long long>(m) * cost(Opcode::INC) <= viaA) {
        reset(target);
        if (value < 0) {
            dec(target, m);
        }
        else {
            inc(target, m);
        }
        return;
    }

    swap(target);
    setConstant(A, value, scratch);
    swap(target);
}

long long Code::setConstantCost(Register target, long long value, bool hasScratch) {
    return planConstant(target, value, hasScratch).cost;
}

void Code::emit(Opcode opcode, Register r) {
    instructions_.push_back({opcode, r, 0});
}

void Code::emitJump(Opcode opcode, Label target) {
    instructions_.push_back({opcode, A, target.id});
}

std::ostream& operator<<(std::ostream& out, const Code& code) {
    for (std::size_t i = 0; i < code.instructions_.size(); i++) {
        const Code::Instruction& instruction = code.instructions_[i];
        out << name(instruction.opcode);

        if (isJump(instruction.opcode)) {
            const auto& position = code.labelPositions_.at(instruction.label);
            if (!position) {
                throw std::logic_error("jump to a label that was never bound");
            }
            out << ' ' << static_cast<long long>(*position) - static_cast<long long>(i);
        }
        else if (hasRegisterOperand(instruction.opcode)) {
            out << ' ' << static_cast<char>('a' + static_cast<int>(instruction.reg));
        }

        out << '\n';
    }

    return out;
}
