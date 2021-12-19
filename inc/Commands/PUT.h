#ifndef PUT_H
#define PUT_H

#include "../Command.h"

class PUT : public Command {
public:
	void execute() override {
		std::cout << "PUT" << std::endl;
	}
};

#endif