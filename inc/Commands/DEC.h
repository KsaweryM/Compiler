#ifndef DEC_H
#define DEC_H

#include "../Command.h"

class DEC : public Command {
private:
	VMregister instructionRegister;

public:
	DEC(VMregister instructionRegister) {
		this->instructionRegister = instructionRegister;
	}

	void execute() {
		std::cout << "DEC " << registerToString(instructionRegister) << std::endl;
	}
};

#endif