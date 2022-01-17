#ifndef POP_H
#define POP_H

#include "commands.h"
#include "../ComplexCommand.h"

class POP : public ComplexCommand {
public:
	POP(long long int n) {
        pushCommandBack(new LOAD(VMregister::h));
        pushCommandBack(new DEC(VMregister::h));
    }
};

#endif