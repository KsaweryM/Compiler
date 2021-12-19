#ifndef INC_H
#define INC_H

#include "../Command.h"

class INC : public Command {
private:
	VMregister instructionRegister;

public:
	INC(VMregister instructionRegister) {
		this->instructionRegister = instructionRegister;
	}

	void execute() {
		std::cout << "INC " << registerToString(instructionRegister) << std::endl;
	}
};

#endif