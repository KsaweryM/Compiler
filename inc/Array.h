#ifndef ARRAY_H
#define ARRAY_H

#include "Data.h"
#include "Variable.h"
#include <vector>

class DataArray : public Data {
private:
int address;
int size;
std::vector<Variable> variables;

public:
    DataArray(int address, int size) {
        for (int i = 0; i < size; i++) {
            variables.push_back(Variable(address));
            address++;
        }
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