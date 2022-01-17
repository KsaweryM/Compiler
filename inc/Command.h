#ifndef COMMAND_H
#define COMMAND_H

#include <iostream>
#include <string>
#include "VMregister.h"
#include <string>

class Command {
protected:
	bool iterator = false;
	int anonymousIndex = -1;

	std::string iteratorName;

	std::string registerToString(VMregister instructionRegister) {
		switch (instructionRegister) {
		case VMregister::a:
			return "a";
		case VMregister::b:
			return "b";
		case VMregister::c:
			return "c";
		case VMregister::d:
			return "d";
		case VMregister::e:
			return "e";
		case VMregister::f:
			return "f";
		case VMregister::g:
			return "g";
		case VMregister::h:
			return "h";
		}

		throw std::invalid_argument("Unknow register!");
	}
public:
	virtual long long int getLength() {
		return 1;
	}

	virtual void execute() = 0;

	virtual ~Command() {

	}

	void setAsIterator() {
		iterator = true;
	}

	bool isIterator() {
		return iterator;
	}

	void setAnonymousIndex(int anonymousIndex) {
		this->anonymousIndex = anonymousIndex;
	}

	int getAnonymousIndex() {
		return anonymousIndex;
	}

	void setIteratorName(std::string iteratorName) {
		this->iteratorName = iteratorName;
	}

	std::string getIteratorName() {
		return iteratorName;
	}
};

#endif
