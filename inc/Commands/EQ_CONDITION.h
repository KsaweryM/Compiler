#ifndef EQ_CONDITION_H
#define EQ_CONDITION_H

#include "commands.h"
#include "../ComplexCommand.h"

class EQ_CONDITION : public ComplexCommand {
public:
	EQ_CONDITION() {
      pushCommandBack(new SAVE_REGISTER_ON_STACK(VMregister::a));
      pushCommandBack(new SAVE_REGISTER_ON_STACK(VMregister::b));

      pushCommandBack(new DECN(VMregister::h, 2));

      pushCommandBack(new LOAD_FROM_STACK_TO_REGISTER(VMregister::a));
      pushCommandBack(new LOAD_FROM_STACK_TO_REGISTER(VMregister::b));

      pushCommandBack(new SUB(VMregister::b));

      pushCommandBack(new INC(VMregister::h));
      pushCommandBack(new STORE(VMregister::h));

      pushCommandBack(new INCN(VMregister::h, 3));

      pushCommandBack(new LOAD_FROM_STACK_TO_REGISTER(VMregister::b));
      pushCommandBack(new LOAD_FROM_STACK_TO_REGISTER(VMregister::a));

      pushCommandBack(new DEC(VMregister::h));
    }
};

#endif