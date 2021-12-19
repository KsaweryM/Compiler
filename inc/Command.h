#ifndef COMMAND_H
#define COMMAND_H

#include <iostream>
#include <string>
#include "VMregister.h"

class Command {
protected:
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
	virtual int getLength() {
		return 1;
	}

	virtual void execute() = 0;

	virtual ~Command() {

	}
};

#endif
