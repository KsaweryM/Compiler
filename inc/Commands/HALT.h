#ifndef HALT_H
#define HALT_H

#include "../Command.h"

class HALT : public Command {
private:
	VMregister instructionRegister;

public:
	void execute() {
		std::cout << "HALT" << std::endl;
	}
};

#endif