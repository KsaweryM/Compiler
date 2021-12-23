#ifndef RESET_STACK_H
#define RESET_STACK_H

#include "../Command.h"

class RESET_STACK : public ComplexCommand {

public:
	RESET_STACK() {
		pushCommandBack(new RESET(VMregister::h));
		pushCommandBack(new DEC(VMregister::h));
	}

};

#endif