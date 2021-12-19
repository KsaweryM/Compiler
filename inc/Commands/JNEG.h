#ifndef JNEG_H
#define JNEG_H

#include "../Command.h"

class JNEG : public Command {
private:
	int k;

public:
	JNEG(int k) {
		this->k = k;
	}

	void execute() {
		std::cout << "JNEG " << k << std::endl;
	}
};

#endif