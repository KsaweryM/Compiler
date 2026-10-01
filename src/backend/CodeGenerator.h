#ifndef BACKEND_CODE_GENERATOR_H
#define BACKEND_CODE_GENERATOR_H

#include "backend/Code.h"
#include "frontend/Ast.h"

// Translates an analyzed program, whose symbols already have addresses, into code for
// the virtual machine.
//
// Register conventions:
//   a     - accumulator: every value is computed here
//   b     - scratch register for loading values and building constants
//   c     - right operand of a binary operation or comparison
//   c-h   - temporaries of the arithmetic routines
//   h     - address of the cell written by an assignment, READ or loop update
//
// No register keeps its value from one command to the next.
class CodeGenerator {
public:
    Code generate(const ast::Program& program);

private:
    void generate(const ast::CommandList& commands);
    void generate(const ast::Assign& assign);
    void generate(const ast::If& statement);
    void generate(const ast::While& loop);
    void generate(const ast::Repeat& loop);
    void generate(const ast::For& loop);
    void generate(const ast::Read& read);
    void generate(const ast::Write& write);

    void evaluate(const ast::Expression& expression);
    void loadOperands(const ast::Value& left, const ast::Value& right);
    void loadValue(const ast::Value& value);
    void loadIdentifier(const ast::Identifier& identifier);
    void loadAddress(const ast::Identifier& identifier, Register target);
    void loadIndexedAddressIntoA(const ast::Identifier& identifier, Register temporary);
    void store(const ast::Identifier& target);

    void addConstant(long long value);
    void subtractConstant(long long value);
    long long addConstantCost(long long value) const;
    bool addByIncrements(long long value) const;
    long long addViaRegisterCost(long long value) const;

    void branch(const ast::Condition& condition, Label target, bool jumpIfTrue);

    Code code_;
};

#endif
