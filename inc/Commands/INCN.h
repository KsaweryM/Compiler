#ifndef INCN_H
#define INCN_H

#include "commands.h"
#include "../ComplexCommand.h"

class INCN : public ComplexCommand {
public:
    INCN(VMregister instructionRegister, long long int k) {
        for (long long int i = 0; i < k; i++) {
            pushCommandBack(new INC(instructionRegister));
        }
    }
};

#endif