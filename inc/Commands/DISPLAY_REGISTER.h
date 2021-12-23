#ifndef DISPLAY_REGISTER_H
#define DISPLAY_REGISTER_H

#include "commands.h"
#include "../ComplexCommand.h"

class DISPLAY_REGISTER : public ComplexCommand {
public:
	DISPLAY_REGISTER(VMregister instructionRegister) {
        pushCommandBack(new SWAP(instructionRegister));
        pushCommandBack(new PUT());
        pushCommandBack(new SWAP(instructionRegister));
    }
};

#endif