#ifndef SWAP_H
#define SWAP_H

#include "../Command.h"

class SWAP : public Command {
private:
	VMregister instructionRegister;

public:
	SWAP(VMregister instructionRegister) {
		this->instructionRegister = instructionRegister;
	}

	void execute() {
		std::cout << "SWAP " << registerToString(instructionRegister) << std::endl;
	}
};

#endif