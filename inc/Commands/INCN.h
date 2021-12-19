#ifndef INCN_H
#define INCN_H

#include "commands.h"
#include "../ComplexCommand.h"

class INCN : public ComplexCommand {
public:
    INCN(VMregister instructionRegister, int k) {
        for (int i = 0; i < k; i++) {
            addCommand(new INC(instructionRegister));
        }
    }
};

#endif