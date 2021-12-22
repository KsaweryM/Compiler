#ifndef PUSH_H
#define PUSH_H

#include "commands.h"
#include "../ComplexCommand.h"

class PUSH : public ComplexCommand {
public:
	PUSH(int n) {
        pushCommandBack(new INC(VMregister::h));
        pushCommandBack(new SAVE_REGISTER_ON_STACK(VMregister::a));
        pushCommandBack(new SAVE_REGISTER_ON_STACK(VMregister::b));
        pushCommandBack(new CREATE_NUMBER(n));
        pushCommandBack(new LOAD_FROM_STACK_TO_REGISTER(VMregister::b));
        pushCommandBack(new DEC(VMregister::h));  
        pushCommandBack(new STORE(VMregister::h));
        pushCommandBack(new INC(VMregister::h));
        pushCommandBack(new LOAD_FROM_STACK_TO_REGISTER(VMregister::a));
    }
};

#endif