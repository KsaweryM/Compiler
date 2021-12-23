#include <iostream>
#include <string>
#include "../inc/Commands/commands.h"
#include "../inc/VMregister.h"
#include "../inc/ParserCommands/commands.h"
#include "../inc/VariableDirector.h"

int main() {
    ComplexCommand* command = new ComplexCommand();

    command->pushCommandBack(new RESET_STACK());
    command->pushCommandBack(new CREATE_TEST_REGISTERS());
    
    VariableDirector* variableDirector = new VariableDirector();

    command->pushCommandBack(variableDirector->declareVariable("a"));
    command->pushCommandBack(variableDirector->declareVariable("b"));
    command->pushCommandBack(variableDirector->declareVariable("c"));

    command->pushCommandBack(variableDirector->assignVariable("a", 10));
    command->pushCommandBack(variableDirector->assignVariable("b", 20));
    command->pushCommandBack(variableDirector->assignVariable("c", 30));

    command->pushCommandBack(new DISPLAY_STACK_N(3));

    command->pushCommandBack(new DISPLAY_REGISTERS());

    command->pushCommandBack(variableDirector->pushVariableOntoStack("c"));

    command->pushCommandBack(variableDirector->assignVariable("c", 300));
    command->pushCommandBack(variableDirector->pushVariableOntoStack("c"));

    command->pushCommandBack(variableDirector->assignVariable("b", 2000));

    command->pushCommandBack(new DISPLAY_STACK_N(5));    

    command->pushCommandBack(new DISPLAY_REGISTERS());
    command->pushCommandBack(new COPY_FROM_STACK_TO_REGISTER(VMregister::a));
    command->pushCommandBack(new PUT());
    
    command->pushCommandBack(new HALT());
    command->execute();

    delete command;
    delete variableDirector;

	return 0;
}