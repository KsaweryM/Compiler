#ifndef GET_H
#define GET_H

#include "../Command.h"

class GET : public Command {
public:
	void execute() override {
		std::cout << "GET" << std::endl;
	}
};


#endif