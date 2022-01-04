#ifndef EQ_CONDITION_H
#define EQ_CONDITION_H

#include "commands.h"
#include "../ComplexCommand.h"

class EQ_CONDITION : public ComplexCommand {
public:
	EQ_CONDITION() {
      pushCommandBack(new SAVE_REGISTER_ON_STACK(VMregister::a));
      pushCommandBack(new SAVE_REGISTER_ON_STACK(VMregister::b));

      pushCommandBack(new SAVE_REGISTER_ON_STACK(VMregister::c));
      pushCommandBack(new SAVE_REGISTER_ON_STACK(VMregister::d));
      
      pushCommandBack(new PUSH(0));
      pushCommandBack(new PUSH(1));

      // c = 1
      pushCommandBack(new LOAD_FROM_STACK_TO_REGISTER(VMregister::c));
      // d = 0
      pushCommandBack(new LOAD_FROM_STACK_TO_REGISTER(VMregister::d));

      pushCommandBack(new DECN(VMregister::h, 4));

      pushCommandBack(new LOAD_FROM_STACK_TO_REGISTER(VMregister::b));
      pushCommandBack(new LOAD_FROM_STACK_TO_REGISTER(VMregister::a));

      pushCommandBack(new SUB(VMregister::b));

      pushCommandBack(new JZERO(3));
      pushCommandBack(new SAVE_REGISTER_ON_STACK(VMregister::d)); // x != y
      pushCommandBack(new JUMP(2));
      pushCommandBack(new SAVE_REGISTER_ON_STACK(VMregister::c)); // x == y
      pushCommandBack(new INCN(VMregister::h, 5));

      pushCommandBack(new LOAD_FROM_STACK_TO_REGISTER(VMregister::d));
      pushCommandBack(new LOAD_FROM_STACK_TO_REGISTER(VMregister::c));
      pushCommandBack(new LOAD_FROM_STACK_TO_REGISTER(VMregister::b));
      pushCommandBack(new LOAD_FROM_STACK_TO_REGISTER(VMregister::a));

      pushCommandBack(new DEC(VMregister::h));
    }
};

#endif