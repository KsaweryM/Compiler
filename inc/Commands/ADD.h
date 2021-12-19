#ifndef ADD_H
#define ADD_H

#include "../Command.h"

class ADD : public Command {
private:
	VMregister instructionRegister;

public:
	ADD(VMregister instructionRegister) {
		this->instructionRegister = instructionRegister;
	}

	void execute() override {
		std::cout << "ADD " << registerToString(instructionRegister) << std::endl;
	}
};

#endif