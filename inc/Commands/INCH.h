#ifndef INCH_H
#define INCH_H

#include "commands.h"
#include "../ComplexCommand.h"

class INCH : public ComplexCommand {
public:
    INCH(long long int k) { // Szybka inkrementacja rejestru stosu
        pushCommandBack(new PUSH(k));
        pushCommandBack(new LOAD_FROM_STACK_TO_REGISTER(VMregister::a));
        pushCommandBack(new ADD(VMregister::h));
        pushCommandBack(new SWAP(VMregister::h));
    }
};

#endif