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
    /*
    INCN(long long int k) { // Szybka inkrementacja rejestru stosu
        pushCommandBack(new PUSH(k));
        pushCommandBack(new LOAD_FROM_STACK_TO_REGISTER(VMregister::a));
        pushCommandBack(new ADD(VMregister::h));
        pushCommandBack(new SWAP(VMregister::h));
    }*/
};

#endif