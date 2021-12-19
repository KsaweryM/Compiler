#include <iostream>
#include <string>

enum VMregister {
	a, b, c, d, e, f, g, h
};

std::string registerToString(VMregister x) {
	switch(x) {
		case a:
			return "a";
		case b:
			return "b";
		case c:
			return "c";
		case d:
			return "d";
		case e:
			return "e";
		case f:
			return "f";
		case g:
			return "g";
		case h:
			return "h";
	}

	throw std::invalid_argument("Unknow register!");
}

void GET() {
	std::cout << "GET" << std::endl;
}

void PUT() {
	std::cout << "PUT" << std::endl;
}

void LOAD(VMregister x) {
	std::cout << "LOAD " << registerToString(x) << std::endl;
}

void STORE(VMregister x) {
	std::cout << "STORE " << registerToString(x) << std::endl;
}

void ADD(VMregister x) {
	std::cout << "ADD " << registerToString(x) << std::endl;
}

void SUB(VMregister x) {
	std::cout << "SUB " << registerToString(x) << std::endl;
}

void SHIFT(VMregister x) {
	std::cout << "SHIFT " << registerToString(x) << std::endl;
}

void SWAP(VMregister x) {
	std::cout << "SWAP " << registerToString(x) << std::endl;
}

void RESET(VMregister x) {
	std::cout << "RESET " << registerToString(x) << std::endl;
}

void INC(VMregister x) {
	std::cout << "INC " << registerToString(x) << std::endl;
}

void INCN(VMregister x, int k) {
    for (int i = 0; i < k; i++) {
	    std::cout << "INC " << registerToString(x) << std::endl;
    }
}

void DEC(VMregister x) {
	std::cout << "DEC " << registerToString(x) << std::endl;
}

void DECN(VMregister x, int k) {
    for (int i = 0; i < k; i++) {
	    std::cout << "DEC " << registerToString(x) << std::endl;
    }
}

void JUMP(int k) {
	std::cout << "JUMP " << k << std::endl;
}

void JPOS(int k) {
	std::cout << "JPOS " << k << std::endl;
}

void JZERO(int k) {
	std::cout << "JZERO " << k << std::endl;
}

void JNEG(int k) {
	std::cout << "JNEG " << k << std::endl;
}

void HALT() {
	std::cout << "HALT" << std::endl;
}

// save register on stack
void SAVE_REGISTER_ON_STACK(VMregister x) {
	INC(h);
	SWAP(x);
	STORE(h);
	SWAP(x);
}

// load number from stack to particular register
void LOAD_FROM_STACK_TO_REGISTER(VMregister x) {
	SWAP(x);
	LOAD(h);
	SWAP(x);
	DEC(h);
}

// copy number from stack to particular register
void COPY_FROM_STACK_TO_REGISTER(VMregister x) {
	SWAP(x);
	LOAD(h);
	SWAP(x);
}

void CREATE_NUMBER(int n) {
	if (n <= 1) {
		RESET(a);
		RESET(b);
		INC(b);

		if (n == 1) {
			ADD(b);
		}
	}
	else {
		CREATE_NUMBER(n / 2);
		SHIFT(b);

		if (n % 2) {
			ADD(b);
		}
	}
}

// push a value to the stack without destroying the values in the registers
void PUSH(int n) {
    INC(h);
    SAVE_REGISTER_ON_STACK(a);
    SAVE_REGISTER_ON_STACK(b);
	CREATE_NUMBER(n);
    LOAD_FROM_STACK_TO_REGISTER(b);
	DEC(h);
    STORE(h);
    INC(h);
    LOAD_FROM_STACK_TO_REGISTER(a);
}

void POP() {
	LOAD(h);
	DEC(h);
}

void ADD_TWO_NUMBERS_FROM_STACK_AND_PUSH_RESULT_ON_STACK() {
	SAVE_REGISTER_ON_STACK(a);
    SAVE_REGISTER_ON_STACK(b);
    
    DECN(h, 2);

    LOAD_FROM_STACK_TO_REGISTER(a);
    LOAD_FROM_STACK_TO_REGISTER(b);

    ADD(b);

    INC(h);
    STORE(h);

    INCN(h, 3);

    LOAD_FROM_STACK_TO_REGISTER(b);
    LOAD_FROM_STACK_TO_REGISTER(a);

    DEC(h);
}

void CHANGE_SIGN_ON_STACK() {
	SAVE_REGISTER_ON_STACK(a);
    SAVE_REGISTER_ON_STACK(b);
    
    DECN(h, 2);

    RESET(a);
    LOAD_FROM_STACK_TO_REGISTER(b);

    SUB(b);
    SAVE_REGISTER_ON_STACK(a);

    INCN(h, 2);

    LOAD_FROM_STACK_TO_REGISTER(b);
    LOAD_FROM_STACK_TO_REGISTER(a);
}

void SUBTRACT_TWO_NUMBERS_FROM_STACK_AND_PUSH_RESULT_ON_STACK() {
	SAVE_REGISTER_ON_STACK(a);
    SAVE_REGISTER_ON_STACK(b);
    
    DECN(h, 2);

    LOAD_FROM_STACK_TO_REGISTER(a);
    LOAD_FROM_STACK_TO_REGISTER(b);

    SWAP(b);
    SUB(b);

    INC(h);
    STORE(h);

    INCN(h, 3);

    LOAD_FROM_STACK_TO_REGISTER(b);
    LOAD_FROM_STACK_TO_REGISTER(a);

    DEC(h);
}

void CLEAR_STACK() {
    RESET(h);
    DEC(h);
}

void DISPLAY_REGISTERS() {
    // display a
    PUT();
    SAVE_REGISTER_ON_STACK(a);
    SWAP(b);

    // display b
    PUT();
    SAVE_REGISTER_ON_STACK(a);
    SWAP(c);

    // display c
    PUT();
    SAVE_REGISTER_ON_STACK(a);
    SWAP(d);

    // display d
    PUT();
    SAVE_REGISTER_ON_STACK(a);
    SWAP(e);

    // display e
    PUT();
    SAVE_REGISTER_ON_STACK(a);
    SWAP(f);
    
    // display f
    PUT();
    SAVE_REGISTER_ON_STACK(a);
    SWAP(g);

    // display g
    PUT();
    SAVE_REGISTER_ON_STACK(a);
    
    LOAD_FROM_STACK_TO_REGISTER(g);
    LOAD_FROM_STACK_TO_REGISTER(f);
    LOAD_FROM_STACK_TO_REGISTER(e);
    LOAD_FROM_STACK_TO_REGISTER(d);
    LOAD_FROM_STACK_TO_REGISTER(c);
    LOAD_FROM_STACK_TO_REGISTER(b);
    LOAD_FROM_STACK_TO_REGISTER(a);
}

void RESET_REAGISTERS() {
    PUSH(0);

    COPY_FROM_STACK_TO_REGISTER(a);
    COPY_FROM_STACK_TO_REGISTER(b);
    COPY_FROM_STACK_TO_REGISTER(c);
    COPY_FROM_STACK_TO_REGISTER(d);
    COPY_FROM_STACK_TO_REGISTER(e);
    COPY_FROM_STACK_TO_REGISTER(f);
    COPY_FROM_STACK_TO_REGISTER(g);
    
    DEC(h);  
}

void TEST_REAGISTERS() {
    PUSH(0);
    LOAD_FROM_STACK_TO_REGISTER(a);

    PUSH(1);
    LOAD_FROM_STACK_TO_REGISTER(b); 

    PUSH(2);
    LOAD_FROM_STACK_TO_REGISTER(c); 

    PUSH(3);
    LOAD_FROM_STACK_TO_REGISTER(d); 

    PUSH(4);
    LOAD_FROM_STACK_TO_REGISTER(e);

    PUSH(5);
    LOAD_FROM_STACK_TO_REGISTER(f);

    PUSH(6);
    LOAD_FROM_STACK_TO_REGISTER(g);
}

void SAVE_NUMBER_TO_REGISTER(VMregister x, int number) {
    PUSH(number);
    LOAD_FROM_STACK_TO_REGISTER(x);
}

// póki co tylko mnożenie dodatnich liczb
void MULTI() {
	POP();
	SWAP(b);
	POP();
	SWAP(d);
	RESET(a);
	SWAP(d);
	JZERO(5);
	SWAP(d);
	ADD(b);
	DEC(d);
	JUMP(-5);
	SWAP(d);
	INC(c);
	STORE(c);
}

int main() {
    CLEAR_STACK();
    RESET_REAGISTERS();
    PUSH(10);
    PUSH(20);
    CHANGE_SIGN_ON_STACK();
    ADD_TWO_NUMBERS_FROM_STACK_AND_PUSH_RESULT_ON_STACK();
    COPY_FROM_STACK_TO_REGISTER(a);
    DISPLAY_REGISTERS();
    HALT();
	return 0;
}