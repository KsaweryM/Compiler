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
	//bool useArray = false;
	bool isVariable = false;
	int containerIndex;
	std::string containerName;
	std::string iteratorName;
	bool uninitialized  = false;

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

	void setAsVariable() {
		isVariable = true;
	}

	bool isThisVariable() {
		return isVariable;
	}

	virtual ~Command() {

	}

	void setAsIterator() {
		iterator = true;
	}
	/*
	void setAsArray() {
		useArray = true;
	}

	void setAsVariable() {
		useArray = false;
	}

	bool isArray() {
		return useArray;
	}

	bool isVariable() {
		return !useArray;
	}
	*/

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

	void setAsInitialized() {
		uninitialized = false;
	}

	bool isUninitialized() {
		return uninitialized;
	}

	void setContainer(std::string containerName, int containerIndex) {
			this->containerName = containerName;
			this->containerIndex = containerIndex;
	}

	std::string getContainerName() {
		return containerName;
	}

	int getContainerIndex() {
		return containerIndex;
	}
};

#endif
