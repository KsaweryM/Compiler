#ifndef JUMP_H
#define JUMP_H

#include "../Command.h"

class JUMP : public Command {
private:
	int k;

public:
	JUMP(int k) {
		this->k = k;
	}

	void execute() {
		std::cout << "JUMP " << k << std::endl;
	}
};

#endif