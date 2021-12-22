#ifndef ADD_TWO_NUMBERS_FROM_STACK_AND_PUSH_RESULT_ON_STACK_H
#define ADD_TWO_NUMBERS_FROM_STACK_AND_PUSH_RESULT_ON_STACK_H

#include "commands.h"
#include "../ComplexCommand.h"

class ADD_TWO_NUMBERS_FROM_STACK_AND_PUSH_RESULT_ON_STACK : public ComplexCommand {
public:
	ADD_TWO_NUMBERS_FROM_STACK_AND_PUSH_RESULT_ON_STACK() {
      pushCommandBack(new SAVE_REGISTER_ON_STACK(VMregister::a));
      pushCommandBack(new SAVE_REGISTER_ON_STACK(VMregister::b));

      pushCommandBack(new DECN(VMregister::h, 2));

      pushCommandBack(new LOAD_FROM_STACK_TO_REGISTER(VMregister::a));
      pushCommandBack(new LOAD_FROM_STACK_TO_REGISTER(VMregister::b));

      pushCommandBack(new ADD(VMregister::b));

      pushCommandBack(new INC(VMregister::h));
      pushCommandBack(new STORE(VMregister::h));

      pushCommandBack(new INCN(VMregister::h, 3));

      pushCommandBack(new LOAD_FROM_STACK_TO_REGISTER(VMregister::b));
      pushCommandBack(new LOAD_FROM_STACK_TO_REGISTER(VMregister::a));

      pushCommandBack(new DEC(VMregister::h));
    }
};

#endif