#ifndef SAVE_REGISTER_ON_STACK_H
#define SAVE_REGISTER_ON_STACK_H

#include "commands.h"
#include "../ComplexCommand.h"

class SAVE_REGISTER_ON_STACK : public ComplexCommand {
public:
	SAVE_REGISTER_ON_STACK(VMregister instructionRegister) {
        addCommand(new INC(VMregister::h));
        addCommand(new SWAP(instructionRegister));
        addCommand(new STORE(VMregister::h));
        addCommand(new SWAP(instructionRegister));
    }
};

#endif