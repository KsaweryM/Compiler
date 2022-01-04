#ifndef WRITE_H
#define WRTIE_H

#include "commands.h"
#include "../ComplexCommand.h"

class WRITE : public ComplexCommand {
public:
	WRITE() {
        pushCommandBack(new SAVE_REGISTER_ON_STACK(VMregister::a));
        pushCommandBack(new DEC(VMregister::h));
        pushCommandBack(new LOAD_FROM_STACK_TO_REGISTER(VMregister::a));
        pushCommandBack(new PUT());
        pushCommandBack(new INCN(VMregister::h, 2));
        pushCommandBack(new LOAD_FROM_STACK_TO_REGISTER(VMregister::a));
        pushCommandBack(new DEC(VMregister::h));
    }
};

#endif