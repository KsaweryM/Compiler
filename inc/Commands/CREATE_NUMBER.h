#ifndef CREATE_NUMBER_H
#define CREATE_NUMBER_H

#include "commands.h"
#include "../ComplexCommand.h"

class CREATE_NUMBER : public ComplexCommand {
private:
    void createNumber(int n) {
        if (n <= 1) {   
            pushCommandBack(new RESET(VMregister::a));
            pushCommandBack(new RESET(VMregister::b));
            pushCommandBack(new INC(VMregister::b));

            if (n == 1) {
                pushCommandBack(new ADD(VMregister::b));
            }
        }
        else {
            createNumber(n / 2);
            pushCommandBack(new SHIFT(VMregister::b));

            if (n % 2) {
                pushCommandBack(new ADD(VMregister::b));
            }
        }
    }

public:
    CREATE_NUMBER(int n) {
        if (n >= 0) {
            createNumber(n);
        }
        else {
            createNumber(-n);
            pushCommandBack(new RESET(VMregister::b));
            pushCommandBack(new SWAP(VMregister::b));
            pushCommandBack(new SUB(VMregister::b));
        }

    }
};

#endif