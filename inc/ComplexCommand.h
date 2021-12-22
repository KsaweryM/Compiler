#ifndef COMPLEX_COMMAND_H
#define COMPLEX_COMMAND_H

#include <deque>
#include "Command.h"

class ComplexCommand : public Command {
private:
	std::deque<Command*> instructions;

public:
	void pushCommandFront(Command* Instruction) {
		instructions.push_front(Instruction);
	}

	void pushCommandBack(Command* Instruction) {
		instructions.push_back(Instruction);
	}

	int getLength() override {
		int length = 0;

		std::deque<Command*>::iterator it;

		for (it = instructions.begin(); it != instructions.end(); it++) {
			length += (*it)->getLength();
		}

		return length;
	}

	void execute() override {
		std::deque<Command*>::iterator it;

		for (it = instructions.begin(); it != instructions.end(); it++) {
			(*it)->execute();
		}
	}

	~ComplexCommand() override {
		std::deque<Command*>::iterator it;

		for (it = instructions.begin(); it != instructions.end(); it++) {
			delete* it;
		}
	}
};

#endif

