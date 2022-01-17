#ifndef DISPLAY_STACK_N_H
#define DISPLAY_STACK_N_H

#include "commands.h"
#include "../ComplexCommand.h"

class DISPLAY_STACK_N : public ComplexCommand {
public:
	DISPLAY_STACK_N(long long int n) {
        for (long long int i = n - 1; i >= 0; i--) {
            pushCommandBack(new PUSH(i));
        }

        pushCommandBack(new SAVE_REGISTER_ON_STACK(VMregister::a));
        pushCommandBack(new SAVE_REGISTER_ON_STACK(VMregister::b));

        pushCommandBack(new DECN(VMregister::h, 2));

        for (long long int i = 0; i < n; i++) {
            pushCommandBack(new LOAD_FROM_STACK_TO_REGISTER(VMregister::b));
            pushCommandBack(new LOAD(VMregister::b));
            pushCommandBack(new PUT());
        }

        pushCommandBack(new INCN(VMregister::h, n + 2));
        pushCommandBack(new LOAD_FROM_STACK_TO_REGISTER(VMregister::b));
        pushCommandBack(new LOAD_FROM_STACK_TO_REGISTER(VMregister::a));

        pushCommandBack(new DECN(VMregister::h, n));
    }
};

#endif