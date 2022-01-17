#ifndef JUMP_H
#define JUMP_H

#include "../Command.h"

class JUMP : public Command {
private:
	long long int k;

public:
	JUMP(long long int k) {
		this->k = k;
	}

	void execute() {
		std::cout << "JUMP " << k << std::endl;
	}
};

#endif