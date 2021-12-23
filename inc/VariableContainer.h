#ifndef VARIABLE_CONTAINER_H
#define VARIABLE_CONTAINER_H

#include "Variable.h"
#include <vector>

class VariableContainer {
private:
    std::vector<Variable*> variables;
    int address;
    int size;

public:
    VariableContainer(int address, int size) {
        for (int i = 0; i < size; i++) {
            variables.push_back(new Variable(address));
            address++;
        }
    }

    ComplexCommand* assign(int index, int value) {
        return variables[index]->assign(value);
    }

    ComplexCommand* pushOnStack(int index) {
        return variables[index]->pushOnStack();
    }

    ~VariableContainer() {
        std::vector<Variable*>::iterator it;

        for (it = variables.begin(); it != variables.end(); it++) {
            delete* it;
        }
    }
};

#endif