#include <iostream>
#include <string>
#include "../inc/Commands/commands.h"
#include "../inc/VMregister.h"
#include "../inc/ParserCommands/commands.h"

int main() {
    ComplexCommand* complex = new ComplexCommand();
    complex->pushCommandBack(new HALT());
    complex->execute();
	return 0;
}