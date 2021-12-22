#ifndef SUBTRACT_TWO_NUMBERS_FROM_STACK_AND_PUSH_RESULT_ON_STACK_H
#define SUBTRACT_TWO_NUMBERS_FROM_STACK_AND_PUSH_RESULT_ON_STACK_H

#include "commands.h"
#include "../ComplexCommand.h"

class SUBTRACT_TWO_NUMBERS_FROM_STACK_AND_PUSH_RESULT_ON_STACK : public ComplexCommand {
public:
	SUBTRACT_TWO_NUMBERS_FROM_STACK_AND_PUSH_RESULT_ON_STACK() {
      addCommand(new SAVE_REGISTER_ON_STACK(VMregister::a));
      addCommand(new SAVE_REGISTER_ON_STACK(VMregister::b));

      addCommand(new DECN(VMregister::h, 2));

      addCommand(new LOAD_FROM_STACK_TO_REGISTER(VMregister::a));
      addCommand(new LOAD_FROM_STACK_TO_REGISTER(VMregister::b));

      addCommand(new SWAP(VMregister::b));
      addCommand(new SUB(VMregister::b));

      addCommand(new INC(VMregister::h));
      addCommand(new STORE(VMregister::h));

      addCommand(new INCN(VMregister::h, 3));

      addCommand(new LOAD_FROM_STACK_TO_REGISTER(VMregister::b));
      addCommand(new LOAD_FROM_STACK_TO_REGISTER(VMregister::a));

      addCommand(new DEC(VMregister::h));
    }
};

#endif