#ifndef GE_CONDITION_H
#define GE_CONDITION_H

#include "commands.h"
#include "../ComplexCommand.h"

class GE_CONDITION : public ComplexCommand {
public:
	GE_CONDITION() {
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
      
      ComplexCommand* isTrue = new ComplexCommand();
      isTrue->pushCommandBack(new SAVE_REGISTER_ON_STACK(VMregister::c));

      ComplexCommand* isFalse = new ComplexCommand();
      isFalse->pushCommandBack(new SAVE_REGISTER_ON_STACK(VMregister::d));
      isFalse->pushCommandBack(new JUMP(isTrue->getLength() + 1));
      
      pushCommandBack(new JPOS(isFalse->getLength() + 1));
      pushCommandBack(isFalse); // x != y
      pushCommandBack(isTrue);  // x == y

      pushCommandBack(new INCN(VMregister::h, 5));

      pushCommandBack(new LOAD_FROM_STACK_TO_REGISTER(VMregister::d));
      pushCommandBack(new LOAD_FROM_STACK_TO_REGISTER(VMregister::c));
      pushCommandBack(new LOAD_FROM_STACK_TO_REGISTER(VMregister::b));
      pushCommandBack(new LOAD_FROM_STACK_TO_REGISTER(VMregister::a));

      pushCommandBack(new DEC(VMregister::h));
    }
};

#endif