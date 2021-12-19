#ifndef COMPLEX_COMMAND_h
#define COMPLEX_COMMAND_h

#include <vector>
#include "Command.h"

class ComplexCommand : public Command {
private:
	std::vector<Command*> instructions;

public:
	void addInstruction(Command* Instruction) {
		instructions.push_back(Instruction);
	}

	int getLength() override {
		int length = 0;

		std::vector<Command*>::iterator it;

		for (it = instructions.begin(); it != instructions.end(); it++) {
			length += (*it)->getLength();
		}

		return length;
	}

	void execute() override {
		std::vector<Command*>::iterator it;

		for (it = instructions.begin(); it != instructions.end(); it++) {
			(*it)->execute();
		}
	}

	~ComplexCommand() override {
		std::vector<Command*>::iterator it;

		for (it = instructions.begin(); it != instructions.end(); it++) {
			delete* it;
		}
	}
};

#endif

