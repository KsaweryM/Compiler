#ifndef SHIFT_H
#define SHIFT_H

#include "../Command.h"

class SHIFT : public Command {
private:
	VMregister instructionRegister;

public:
	SHIFT(VMregister instructionRegister) {
		this->instructionRegister = instructionRegister;
	}

	void execute() {
		std::cout << "SHIFT " << registerToString(instructionRegister) << std::endl;
	}
};

#endif