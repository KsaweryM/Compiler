#ifndef JPOS_H
#define JPOS_H

#include "../Command.h"

class JPOS : public Command {
private:
	int k;

public:
	JPOS(int k) {
		this->k = k;
	}

	void execute() {
		std::cout << "JPOS " << k << std::endl;
	}
};

#endif