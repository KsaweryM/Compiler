#ifndef CREATE_NUMBER_H
#define CREATE_NUMBER_H

#include "commands.h"
#include "../ComplexCommand.h"

class CREATE_NUMBER : public ComplexCommand {
private:
    void createNumber(int n) {
        if (n <= 1) {   
            addCommand(new RESET(VMregister::a));
            addCommand(new RESET(VMregister::b));
            addCommand(new INC(VMregister::b));

            if (n == 1) {
                addCommand(new ADD(VMregister::b));
            }
        }
        else {
            CREATE_NUMBER(n / 2);
            addCommand(new SHIFT(VMregister::b));

            if (n % 2) {
                addCommand(new ADD(VMregister::b));
            }
        }
    }

public:
    CREATE_NUMBER(int n) {
        createNumber(n);
    }
};

#endif