#ifndef RESET_H
#define RESET_H

#include "../Command.h"

class RESET : public Command {
private:
	VMregister instructionRegister;

public:
	RESET(VMregister instructionRegister) {
		this->instructionRegister = instructionRegister;
	}

	void execute() {
		std::cout << "RESET " << registerToString(instructionRegister) << std::endl;
	}
};

#endif