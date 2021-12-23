#ifndef VARIABLE_CONTAINER_H
#define VARIABLE_CONTAINER_H

#include "Variable.h"
#include <vector>

class VariableContainer {
private:
    std::vector<Variable*> variables;
    int address;
    int firstIndex;
    int lastIndex;
    int size;

public:
    VariableContainer(int address, int firstIndex, int lastIndex) {
        this->firstIndex = firstIndex;
        this->lastIndex = lastIndex;
        this->size = lastIndex - firstIndex + 1;

        for (int i = 0; i < size; i++) {
            variables.push_back(new Variable(address));
            address++;
        }
    }

    ComplexCommand* assign(int index, int value) {
        return variables[index - firstIndex]->assign(value);
    }

    ComplexCommand* assignByValueFromStack(int index) {
        return variables[index - firstIndex]->assignByValueFromStack();
    }

    ComplexCommand* pushOnStack(int index) {
        return variables[index - firstIndex]->pushOnStack();
    }

    ~VariableContainer() {
        std::vector<Variable*>::iterator it;

        for (it = variables.begin(); it != variables.end(); it++) {
            delete* it;
        }
    }
};

#endif