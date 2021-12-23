#ifndef DISPLAY_REGISTERS_H
#define DISPLAY_REGISTERS_H

#include "commands.h"
#include "../ComplexCommand.h"

class DISPLAY_REGISTERS : public ComplexCommand {
public:
	DISPLAY_REGISTERS() {
        pushCommandBack(new PUT());
        pushCommandBack(new SAVE_REGISTER_ON_STACK(VMregister::a));
        pushCommandBack(new SWAP(VMregister::b));

        // display b
        pushCommandBack(new PUT());
        pushCommandBack(new SAVE_REGISTER_ON_STACK(VMregister::a));
        pushCommandBack(new SWAP(VMregister::c));

        // display c
        pushCommandBack(new PUT());
        pushCommandBack(new SAVE_REGISTER_ON_STACK(VMregister::a));
        pushCommandBack(new SWAP(VMregister::d));

        // display d
        pushCommandBack(new PUT());
        pushCommandBack(new SAVE_REGISTER_ON_STACK(VMregister::a));
        pushCommandBack(new SWAP(VMregister::e));

        // display e
        pushCommandBack(new PUT());
        pushCommandBack(new SAVE_REGISTER_ON_STACK(VMregister::a));
        pushCommandBack(new SWAP(VMregister::f));
        
        // display f
        pushCommandBack(new PUT());
        pushCommandBack(new SAVE_REGISTER_ON_STACK(VMregister::a));
        pushCommandBack(new SWAP(VMregister::g));

        // display g
        pushCommandBack(new PUT());
        pushCommandBack(new SAVE_REGISTER_ON_STACK(VMregister::a));
        
        pushCommandBack(new LOAD_FROM_STACK_TO_REGISTER(VMregister::g));
        pushCommandBack(new LOAD_FROM_STACK_TO_REGISTER(VMregister::f));
        pushCommandBack(new LOAD_FROM_STACK_TO_REGISTER(VMregister::e));
        pushCommandBack(new LOAD_FROM_STACK_TO_REGISTER(VMregister::d));
        pushCommandBack(new LOAD_FROM_STACK_TO_REGISTER(VMregister::c));
        pushCommandBack(new LOAD_FROM_STACK_TO_REGISTER(VMregister::b));
        pushCommandBack(new LOAD_FROM_STACK_TO_REGISTER(VMregister::a));
    }
};

#endif