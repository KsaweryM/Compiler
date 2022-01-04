#ifndef READ_H
#define READ_H

#include "commands.h"
#include "../ComplexCommand.h"

class READ : public ComplexCommand {
public:
	READ() {
        pushCommandBack(new SAVE_REGISTER_ON_STACK(VMregister::a));
        pushCommandBack(new SAVE_REGISTER_ON_STACK(VMregister::b));

        pushCommandBack(new DECN(VMregister::h, 2));
        
        pushCommandBack(new LOAD_FROM_STACK_TO_REGISTER(VMregister::b));
        pushCommandBack(new GET());

        pushCommandBack(new STORE(VMregister::b));

        pushCommandBack(new INCN(VMregister::h, 3));
        
        pushCommandBack(new LOAD_FROM_STACK_TO_REGISTER(VMregister::b));
        
        pushCommandBack(new LOAD_FROM_STACK_TO_REGISTER(VMregister::a));

        pushCommandBack(new DEC(VMregister::h));
    }
};

#endif