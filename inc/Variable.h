#ifndef VARIABLE_H
#define VARIABLE_H

#include <string>
#include "VMregister.h"
#include "Data.h"
#include "Commands/commands.h"
#include "ComplexCommand.h"

class Variable : public Data {
private:
    int address;
    bool isInitialized;

public:
    Variable(int address) {
        this->address = address;
        isInitialized = false;
    }

    int getAddress() {
        return address;
    }

    ComplexCommand* assign(int value) override {
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
};

#endif