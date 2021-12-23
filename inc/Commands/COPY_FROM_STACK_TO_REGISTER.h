#ifndef COPY_FROM_STACK_TO_REGISTER_H
#define COPY_FROM_STACK_TO_REGISTER_H

#include "commands.h"
#include "../ComplexCommand.h"

class COPY_FROM_STACK_TO_REGISTER : public ComplexCommand {
public:
	COPY_FROM_STACK_TO_REGISTER(VMregister instructionRegister) {
        pushCommandBack(new SWAP(instructionRegister));
        pushCommandBack(new LOAD(VMregister::h));
        pushCommandBack(new SWAP(instructionRegister));
    }
};

#endif