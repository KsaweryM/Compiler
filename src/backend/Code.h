#ifndef BACKEND_CODE_H
#define BACKEND_CODE_H

#include <cstddef>
#include <optional>
#include <ostream>
#include <vector>

// Registers of the virtual machine. Arithmetic and memory access go through register a.
enum class Register { a, b, c, d, e, f, g, h };

// Short names for writing instruction sequences, e.g. `code.add(B)`.
namespace registers {
inline constexpr Register A = Register::a;
inline constexpr Register B = Register::b;
inline constexpr Register C = Register::c;
inline constexpr Register D = Register::d;
inline constexpr Register E = Register::e;
inline constexpr Register F = Register::f;
inline constexpr Register G = Register::g;
inline constexpr Register H = Register::h;
}

enum class Opcode { GET, PUT, LOAD, STORE, ADD, SUB, SHIFT, SWAP, RESET, INC, DEC, JUMP, JPOS, JZERO, JNEG, HALT };

// Cost the virtual machine charges for executing one instruction.
long long cost(Opcode opcode);

// A jump target, created by Code::newLabel and placed by Code::bind.
struct Label {
    std::size_t id;
};

// A program for the virtual machine under construction. Jumps refer to labels and are
// turned into relative offsets when the program is written out.
class Code {
public:
    Label newLabel();
    void bind(Label label);

    // Machine instructions; see docs/architecture.md for their semantics.
    void get();
    void put();
    void load(Register r);
    void store(Register r);
    void add(Register r);
    void sub(Register r);
    void shift(Register r);
    void swap(Register r);
    void reset(Register r);
    void inc(Register r);
    void dec(Register r);
    void jump(Label target);
    void jpos(Label target);
    void jzero(Label target);
    void jneg(Label target);
    void halt();

    void inc(Register r, unsigned long long times);
    void dec(Register r, unsigned long long times);

    // Sets `target` to `value` with the cheapest instruction sequence. Clobbers register a
    // and `scratch` (used to hold a shift amount); `scratch` may be the same as `target`.
    void setConstant(Register target, long long value, std::optional<Register> scratch = std::nullopt);

    // Like setConstant, but keeps register a intact. `target` must not be register a and
    // `scratch` must differ from `target`.
    void setConstantPreservingA(Register target, long long value, std::optional<Register> scratch = std::nullopt);

    // Cost of setConstant(target, value, scratch) with or without a scratch register.
    static long long setConstantCost(Register target, long long value, bool hasScratch);

    std::size_t size() const { return instructions_.size(); }

    friend std::ostream& operator<<(std::ostream& out, const Code& code);

private:
    struct Instruction {
        Opcode opcode;
        Register reg;
        std::size_t label;
    };

    void emit(Opcode opcode, Register r = Register::a);
    void emitJump(Opcode opcode, Label target);

    std::vector<Instruction> instructions_;
    std::vector<std::optional<std::size_t>> labelPositions_;
};

#endif
