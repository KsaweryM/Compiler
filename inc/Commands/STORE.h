#ifndef STORE_H
#define STORE_H

#include "../Command.h"

class STORE : public Command {
private:
	VMregister instructionRegister;

public:
	STORE(VMregister instructionRegister) {
		this->instructionRegister = instructionRegister;
	}

	void execute() override {
		std::cout << "STORE " << registerToString(instructionRegister) << std::endl;
	}
};

#endif