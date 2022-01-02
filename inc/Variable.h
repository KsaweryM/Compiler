#ifndef VARIABLE_H
#define VARIABLE_H

#include <string>
#include "Commands/commands.h"

class Variable {
private:
    int address;
    bool isInitialized;

public:
    Variable(int address) {
        this->address = address;
        isInitialized = false;
    }

    ComplexCommand* pushAddressOntoStack() {
        return new PUSH(address);
    }

    /*
    ComplexCommand* assign(int value) {
        ComplexCommand* command = new ComplexCommand();
        
        command->pushCommandBack(new SAVE_REGISTER_ON_STACK(VMregister::a));
        command->pushCommandBack(new SAVE_REGISTER_ON_STACK(VMregister::b));
        
        command->pushCommandBack(new PUSH(value));
        command->pushCommandBack(new PUSH(address));

        command->pushCommandBack(new LOAD_FROM_STACK_TO_REGISTER(VMregister::b));
        command->pushCommandBack(new LOAD_FROM_STACK_TO_REGISTER(VMregister::a));
        
        command->pushCommandBack(new STORE(VMregister::b));

        command->pushCommandBack(new LOAD_FROM_STACK_TO_REGISTER(VMregister::b));
        command->pushCommandBack(new LOAD_FROM_STACK_TO_REGISTER(VMregister::a));

        return command;
    }
    */

   /*
    ComplexCommand* assignByValueFromStack() {
        ComplexCommand* command = new ComplexCommand();

        command->pushCommandBack(new SAVE_REGISTER_ON_STACK(VMregister::a));
        command->pushCommandBack(new SAVE_REGISTER_ON_STACK(VMregister::b));

        command->pushCommandBack(new PUSH(address));

        command->pushCommandBack(new LOAD_FROM_STACK_TO_REGISTER(VMregister::b));
        
        command->pushCommandBack(new DECN(VMregister::h, 2));

        command->pushCommandBack(new COPY_FROM_STACK_TO_REGISTER(VMregister::a));

        command->pushCommandBack(new STORE(VMregister::b));

        command->pushCommandBack(new INCN(VMregister::h, 2));

        command->pushCommandBack(new LOAD_FROM_STACK_TO_REGISTER(VMregister::b));
        command->pushCommandBack(new LOAD_FROM_STACK_TO_REGISTER(VMregister::a));

        command->pushCommandBack(new DEC(VMregister::h));        

        return command;
    }
    */

    ComplexCommand* pushOnStack() {
        ComplexCommand* command = new ComplexCommand();

/*
        command->pushCommandBack(new INC(VMregister::h));
        command->pushCommandBack(new SAVE_REGISTER_ON_STACK(VMregister::a));
        command->pushCommandBack(new SAVE_REGISTER_ON_STACK(VMregister::b));
        
        command->pushCommandBack(new PUSH(address));
        command->pushCommandBack(new LOAD_FROM_STACK_TO_REGISTER(VMregister::b));
        command->pushCommandBack(new LOAD(VMregister::b));
        command->pushCommandBack(new DECN(VMregister::h, 2));
        command->pushCommandBack(new STORE(VMregister::h));
        command->pushCommandBack(new INCN(VMregister::h, 2));

        command->pushCommandBack(new LOAD_FROM_STACK_TO_REGISTER(VMregister::b));
        command->pushCommandBack(new LOAD_FROM_STACK_TO_REGISTER(VMregister::a));
        */

        return command;
    }
};

#endif