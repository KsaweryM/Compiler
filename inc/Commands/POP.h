#ifndef POP_H
#define POP_H

#include "commands.h"
#include "../ComplexCommand.h"

class POP : public ComplexCommand {
public:
	POP(int n) {
        pushCommandBack(new LOAD(VMregister::h));
        pushCommandBack(new DEC(VMregister::h));
    }
};

#endif