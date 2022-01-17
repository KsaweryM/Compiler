#ifndef JZERO_H
#define JZERO_H

#include "../Command.h"

class JZERO : public Command {
private:
	long long int k;

public:
	JZERO(long long int k) {
		this->k = k;
	}

	void execute() {
		std::cout << "JZERO " << k << std::endl;
	}
};

#endif