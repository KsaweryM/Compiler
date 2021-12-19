#ifndef SUB_H
#define SUB_H

#include "../Command.h"

class SUB : public Command {
private:
	VMregister instructionRegister;

public:
	SUB(VMregister instructionRegister) {
		this->instructionRegister = instructionRegister;
	}

	void execute() {
		std::cout << "SUB " << registerToString(instructionRegister) << std::endl;
	}
};

#endif
