#ifndef POP_H
#define POP_H

#include "commands.h"
#include "../ComplexCommand.h"

class POP : public ComplexCommand {
public:
	POP(int n) {
        addCommand(new LOAD(VMregister::h));
        addCommand(new DEC(VMregister::h));
    }
};

#endif