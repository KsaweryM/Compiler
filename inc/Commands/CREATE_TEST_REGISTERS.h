#ifndef CREATE_TEST_REGISTERS_H
#define CREATE_TEST_REGISTERS_H

#include "commands.h"
#include "../ComplexCommand.h"

class CREATE_TEST_REGISTERS : public ComplexCommand {
public:
	CREATE_TEST_REGISTERS() {
        pushCommandBack(new PUSH(0));
        pushCommandBack(new LOAD_FROM_STACK_TO_REGISTER(VMregister::a));

        pushCommandBack(new PUSH(1));
        pushCommandBack(new LOAD_FROM_STACK_TO_REGISTER(VMregister::b));

        pushCommandBack(new PUSH(2));
        pushCommandBack(new LOAD_FROM_STACK_TO_REGISTER(VMregister::c));

        pushCommandBack(new PUSH(3));
        pushCommandBack(new LOAD_FROM_STACK_TO_REGISTER(VMregister::d));

        pushCommandBack(new PUSH(4));
        pushCommandBack(new LOAD_FROM_STACK_TO_REGISTER(VMregister::e));

        pushCommandBack(new PUSH(5));
        pushCommandBack(new LOAD_FROM_STACK_TO_REGISTER(VMregister::f));

        pushCommandBack(new PUSH(6));
        pushCommandBack(new LOAD_FROM_STACK_TO_REGISTER(VMregister::g));
    }
};

#endif