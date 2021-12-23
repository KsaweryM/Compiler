#ifndef VARIABLE_DIRECTOR_H
#define VARIABLE_DIRECTOR_H

#include <string>
#include <map>

#include "VariableContainer.h"
#include "ComplexCommand.h"

class VariableDirector {
private:
    int stack = 0;
    std::map<std::string, VariableContainer*> variableContainers;

public:
    Command* declareVariable(std::string name) {
        variableContainers[name] = new VariableContainer(stack, 0, 0);
        stack += 1;

        return new INC(VMregister::h);
    }

    Command* declareArray(std::string name, int firstIndex, int lastIndex) {
        variableContainers[name] = new VariableContainer(stack, firstIndex, lastIndex);
        int size = lastIndex - firstIndex + 1;

        stack += size;

        return new INCN(VMregister::h, size);      
    }

    Command* assignVariable(std::string name, int value) {
        return variableContainers[name]->assign(0, value);
    }

    Command* assignVariableByValueFromStack(std::string name) {
        return variableContainers[name]->assignByValueFromStack(0);
    }

    Command* assignVariableFromArray(std::string name, int index, int value) {
        return variableContainers[name]->assign(index, value);
    }

    Command* assignVariableFromArrayByValueFromStack(std::string name, int index) {
        return variableContainers[name]->assignByValueFromStack(index);
    }
    

    Command* pushVariableOntoStack(std::string name) {
        return variableContainers[name]->pushOnStack(0);
    }

    Command* pushVariableFromArrayOntoStack(std::string name, int index) {
        return variableContainers[name]->pushOnStack(index);
    }

    ~VariableDirector() {
        std::map<std::string, VariableContainer*>::iterator it;

		for (it = variableContainers.begin(); it != variableContainers.end(); it++) {
			delete it->second;
		}
    }
};

#endif