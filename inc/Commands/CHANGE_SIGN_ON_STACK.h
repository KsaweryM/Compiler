#ifndef CHANGE_SIGN_ON_STACK_H
#define CHANGE_SIGN_ON_STACK_H

#include "commands.h"
#include "../ComplexCommand.h"

class CHANGE_SIGN_ON_STACK : public ComplexCommand {
public:
	CHANGE_SIGN_ON_STACK() {
        pushCommandBack(new SAVE_REGISTER_ON_STACK(VMregister::a));
        pushCommandBack(new SAVE_REGISTER_ON_STACK(VMregister::b));

        pushCommandBack(new DECN(VMregister::h, 2));

        pushCommandBack(new RESET(VMregister::a));
        pushCommandBack(new LOAD_FROM_STACK_TO_REGISTER(VMregister::b));

        pushCommandBack(new SUB(VMregister::b));
        pushCommandBack(new SAVE_REGISTER_ON_STACK(VMregister::a));

        pushCommandBack(new INCN(VMregister::h, 2));

        pushCommandBack(new LOAD_FROM_STACK_TO_REGISTER(VMregister::b));
        pushCommandBack(new LOAD_FROM_STACK_TO_REGISTER(VMregister::a));
    }
};

#endif