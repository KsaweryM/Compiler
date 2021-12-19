#ifndef PUSH_H
#define PUSH_H

#include "commands.h"
#include "../ComplexCommand.h"

class PUSH : public ComplexCommand {
public:
	PUSH(int n) {
        addCommand(new INC(VMregister::h));
        addCommand(new SAVE_REGISTER_ON_STACK(VMregister::a));
        addCommand(new SAVE_REGISTER_ON_STACK(VMregister::b));
        addCommand(new CREATE_NUMBER(n));
        addCommand(new LOAD_FROM_STACK_TO_REGISTER(VMregister::b));
        addCommand(new DEC(VMregister::h));  
        //TOOD      
    }
};

#endif

/*
void PUSH(int n) {
    INC(h);
    SAVE_REGISTER_ON_STACK(a);
    SAVE_REGISTER_ON_STACK(b);
	CREATE_NUMBER(n);
    LOAD_FROM_STACK_TO_REGISTER(b);
	DEC(h);
    STORE(h);
    INC(h);
    LOAD_FROM_STACK_TO_REGISTER(a);
}
*/