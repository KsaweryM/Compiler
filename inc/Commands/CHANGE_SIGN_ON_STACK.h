#ifndef CHANGE_SIGN_ON_STACK_H
#define CHANGE_SIGN_ON_STACK_H

#include "commands.h"
#include "../ComplexCommand.h"

class CHANGE_SIGN_ON_STACK : public ComplexCommand {
public:
	CHANGE_SIGN_ON_STACK() {
        addCommand(new SAVE_REGISTER_ON_STACK(VMregister::a));
        addCommand(new SAVE_REGISTER_ON_STACK(VMregister::b));

        addCommand(new DECN(VMregister::h, 2));

        addCommand(new RESET(VMregister::a));
        addCommand(new LOAD_FROM_STACK_TO_REGISTER(VMregister::b));

        addCommand(new SUB(VMregister::b));
        addCommand(new SAVE_REGISTER_ON_STACK(VMregister::a));

        addCommand(new INCN(VMregister::h, 2));

        addCommand(new LOAD_FROM_STACK_TO_REGISTER(VMregister::b));
        addCommand(new LOAD_FROM_STACK_TO_REGISTER(VMregister::a));
    }
};

#endif