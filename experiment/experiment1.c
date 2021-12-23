#include <iostream>
#include <string>
#include "../inc/Commands/commands.h"
#include "../inc/VMregister.h"
#include "../inc/VariableDirector.h"

int main() {
    ComplexCommand* command = new ComplexCommand();

    command->pushCommandBack(new RESET_STACK());
    command->pushCommandBack(new CREATE_TEST_REGISTERS());
    
    VariableDirector* variableDirector = new VariableDirector();

    command->pushCommandBack(variableDirector->declareVariable("a"));
    command->pushCommandBack(variableDirector->declareVariable("b"));
    command->pushCommandBack(variableDirector->declareVariable("c"));

    
    command->pushCommandBack(variableDirector->declareArray("ABC", 0, 2));
    
    command->pushCommandBack(variableDirector->assignVariable("a", 10));
    command->pushCommandBack(variableDirector->assignVariable("b", 20));
    command->pushCommandBack(variableDirector->assignVariable("c", 30));

    
    command->pushCommandBack(variableDirector->assignVariableFromArray("ABC", 0, 100));
    command->pushCommandBack(variableDirector->assignVariableFromArray("ABC", 1, 200));
    command->pushCommandBack(variableDirector->assignVariableFromArray("ABC", 2, 300));
    
    command->pushCommandBack(variableDirector->pushVariableFromArrayOntoStack("ABC", 0));

    command->pushCommandBack(variableDirector->assignVariableFromArray("ABC", 0, 1000));

    command->pushCommandBack(new DISPLAY_STACK_N(7));
    command->pushCommandBack(new DISPLAY_REGISTERS());
    
    command->pushCommandBack(new HALT());
    command->execute();

    delete command;
    delete variableDirector;

	return 0;
}