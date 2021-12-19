#include <iostream>
#include <string>
#include <vector>

using namespace std;

enum class VMregister {
	a, b, c, d, e, f, g, h
};

class Instruction {
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

	virtual ~Instruction() {

	}
};

class ComplexInstruction : public Instruction {
private:
	std::vector<Instruction*> instructions;

public:
	void addInstruction(Instruction* Instruction) {
		instructions.push_back(Instruction);
	}

	int getLength() override {
		int length = 0;

		std::vector<Instruction*>::iterator it;

		for (it = instructions.begin(); it != instructions.end(); it++) {
			length += (*it)->getLength();
		}

		return length;
	}

	void execute() override {
		std::vector<Instruction*>::iterator it;

		for (it = instructions.begin(); it != instructions.end(); it++) {
			(*it)->execute();
		}
	}

	~ComplexInstruction() override {
		std::vector<Instruction*>::iterator it;

		for (it = instructions.begin(); it != instructions.end(); it++) {
			delete *it;
		}
	}
};

class GET : public Instruction {
public:
	void execute() override {
		std::cout << "GET" << std::endl;
	}
};

class PUT : public Instruction {
public:
	void execute() override {
		std::cout << "PUT" << std::endl;
	}
};

class LOAD : public Instruction {
private:
	VMregister instructionRegister;

public:
	LOAD(VMregister instructionRegister) {
		this->instructionRegister = instructionRegister;
	}

	void execute() override {
		std::cout << "PUT " << registerToString(instructionRegister) << std::endl;
	}
};

class STORE : public Instruction {
private:
	VMregister instructionRegister;

public:
	STORE(VMregister instructionRegister) {
		this->instructionRegister = instructionRegister;
	}

	void execute() override {
		std::cout << "STORE " << registerToString(instructionRegister) << std::endl;
	}
};

class ADD : public Instruction {
private:
	VMregister instructionRegister;

public:
	ADD(VMregister instructionRegister) {
		this->instructionRegister = instructionRegister;
	}

	void execute() override {
		std::cout << "ADD " << registerToString(instructionRegister) << std::endl;
	}
};

class SUB : public Instruction {
private:
	VMregister instructionRegister;

public:
	SUB(VMregister instructionRegister) {
		this->instructionRegister = instructionRegister;
	}

	void execute() {
		std::cout << "SUB " << registerToString(instructionRegister) << std::endl;
	}
};

class SHIFT : public Instruction {
private:
	VMregister instructionRegister;

public:
	SHIFT(VMregister instructionRegister) {
		this->instructionRegister = instructionRegister;
	}

	void execute() {
		std::cout << "SHIFT " << registerToString(instructionRegister) << std::endl;
	}
};

class SWAP : public Instruction {
private:
	VMregister instructionRegister;

public:
	SWAP(VMregister instructionRegister) {
		this->instructionRegister = instructionRegister;
	}

	void execute() {
		std::cout << "SWAP " << registerToString(instructionRegister) << std::endl;
	}
};

class RESET : public Instruction {
private:
	VMregister instructionRegister;

public:
	RESET(VMregister instructionRegister) {
		this->instructionRegister = instructionRegister;
	}

	void execute() {
		std::cout << "RESET " << registerToString(instructionRegister) << std::endl;
	}
};

class INC : public Instruction {
private:
	VMregister instructionRegister;

public:
	INC(VMregister instructionRegister) {
		this->instructionRegister = instructionRegister;
	}

	void execute() {
		std::cout << "INC " << registerToString(instructionRegister) << std::endl;
	}
};

class DEC : public Instruction {
private:
	VMregister instructionRegister;

public:
	DEC(VMregister instructionRegister) {
		this->instructionRegister = instructionRegister;
	}

	void execute() {
		std::cout << "DEC " << registerToString(instructionRegister) << std::endl;
	}
};

/*
void INCN(VMregister x, int k) {
	for (int i = 0; i < k; i++) {
		std::cout << "INC " << registerToString(x) << std::endl;
	}
}

void DECN(VMregister x, int k) {
	for (int i = 0; i < k; i++) {
		std::cout << "DEC " << registerToString(x) << std::endl;
	}
}
*/

class JUMP : public Instruction {
private:
	int k;

public:
	JUMP(int k) {
		this->k = k;
	}

	void execute() {
		std::cout << "JUMP " << k << std::endl;
	}
};

class JPOS : public Instruction {
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

class JZERO : public Instruction {
private:
	int k;

public:
	JZERO(int k) {
		this->k = k;
	}

	void execute() {
		std::cout << "JZERO " << k << std::endl;
	}
};

class JNEG : public Instruction {
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

class HALT : public Instruction {
private:
	VMregister instructionRegister;

public:
	void execute() {
		std::cout << "HALT" << std::endl;
	}
};


int main() {

	ComplexInstruction* complex = new ComplexInstruction;

	complex->addInstruction(new SUB(VMregister::a));
	complex->addInstruction(new ADD(VMregister::b));


	ComplexInstruction* secondComplex = new ComplexInstruction;

	secondComplex->addInstruction(complex);
	secondComplex->addInstruction(new SUB(VMregister::c));
	secondComplex->addInstruction(new ADD(VMregister::c));
	secondComplex->addInstruction(new HALT());
	
	secondComplex->execute();

	delete secondComplex;

	return 0;
}