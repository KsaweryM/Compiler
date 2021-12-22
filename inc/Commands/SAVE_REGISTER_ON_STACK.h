#ifndef SAVE_REGISTER_ON_STACK_H
#define SAVE_REGISTER_ON_STACK_H

#include "commands.h"
#include "../ComplexCommand.h"

class SAVE_REGISTER_ON_STACK : public ComplexCommand {
public:
	SAVE_REGISTER_ON_STACK(VMregister instructionRegister) {
        pushCommandBack(new INC(VMregister::h));
        pushCommandBack(new SWAP(instructionRegister));
        pushCommandBack(new STORE(VMregister::h));
        pushCommandBack(new SWAP(instructionRegister));
    }
};

#endif