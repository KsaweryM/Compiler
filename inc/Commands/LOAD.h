#ifndef LOAD_H
#define LOAD_H

#include "../Command.h"

class LOAD : public Command {
private:
	VMregister instructionRegister;

public:
	LOAD(VMregister instructionRegister) {
		this->instructionRegister = instructionRegister;
	}

	void execute() override {
		std::cout << "LOAD " << registerToString(instructionRegister) << std::endl;
	}
};

#endif