#ifndef DATA_TABLE_H
#define DATA_TABLE_H

#include <string>
#include <map>
#include "Variable.h"
#include "Array.h"
#include "ComplexCommand.h"

class DataTable {
private:
    int stackTop;
    std::map<std::string, Data*> data;

public:
    DataTable() {
        stackTop = 0;
    }

    Command* declareVariable(std::string variableName) {
        stackTop += 1;
        
        data[variableName] = new Variable(stackTop);

        return new INC(VMregister::h);
    }

    void declareArray(std::string arrayName, int size) {
        data[arrayName] = new DataArray(stackTop, size);
        stackTop += size;       
    }

    Command* assignVariable(std::string variableName, int value) {
        return data[variableName]->assign(value);
    }

    Command* assignVariableInArray(std::string arrayName, int index, int value) {
        return 0;
    }
};

#endif