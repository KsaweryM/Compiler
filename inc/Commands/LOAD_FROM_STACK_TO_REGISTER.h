#ifndef LOAD_FROM_STACK_TO_REGISTER_H
#define LOAD_FROM_STACK_TO_REGISTER_H

#include "commands.h"
#include "../ComplexCommand.h"

class LOAD_FROM_STACK_TO_REGISTER : public ComplexCommand {
public:
	LOAD_FROM_STACK_TO_REGISTER(VMregister instructionRegister) {
        addCommand(new SWAP(instructionRegister));
        addCommand(new LOAD(VMregister::h));
        addCommand(new SWAP(instructionRegister));
        addCommand(new DEC(VMregister::h));
    }
};

#endif