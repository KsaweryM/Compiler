#ifndef DECN_H
#define DECN_H

#include "commands.h"
#include "../ComplexCommand.h"

class DECN : public ComplexCommand {
public:
    DECN(VMregister instructionRegister, long long int k) {
        for (long long int i = 0; i < k; i++) {
            pushCommandBack(new DEC(instructionRegister));
        }
    }
};

#endif