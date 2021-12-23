#ifndef RESET_REAGISTERS_H
#define RESET_REAGISTERS_H

#include "commands.h"
#include "../ComplexCommand.h"

class RESET_REAGISTERS : public ComplexCommand {
public:
	RESET_REAGISTERS() {
        pushCommandBack(new PUSH(0));

        pushCommandBack(new COPY_FROM_STACK_TO_REGISTER(VMregister::a));
        pushCommandBack(new COPY_FROM_STACK_TO_REGISTER(VMregister::b));
        pushCommandBack(new COPY_FROM_STACK_TO_REGISTER(VMregister::c));
        pushCommandBack(new COPY_FROM_STACK_TO_REGISTER(VMregister::d));
        pushCommandBack(new COPY_FROM_STACK_TO_REGISTER(VMregister::e));
        pushCommandBack(new COPY_FROM_STACK_TO_REGISTER(VMregister::f));
        pushCommandBack(new COPY_FROM_STACK_TO_REGISTER(VMregister::g));
        
        pushCommandBack(new DEC(VMregister::h));
    }
};

#endif