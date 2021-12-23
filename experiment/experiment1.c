#include <iostream>
#include <string>
#include "../inc/Commands/commands.h"
#include "../inc/VMregister.h"
#include "../inc/ParserCommands/commands.h"
#include "../inc/DataTable.h"

int main() {
    ComplexCommand* command = new ComplexCommand();
    command->pushCommandBack(new RESET_STACK());
    command->pushCommandBack(new CREATE_TEST_REGISTERS());
    
    DataTable* table = new DataTable();

    command->pushCommandBack(table->declareVariable("a"));
    command->pushCommandBack(table->declareVariable("b"));
    command->pushCommandBack(table->declareVariable("c"));
    command->pushCommandBack(table->assignVariable("a", 10));
    command->pushCommandBack(table->assignVariable("b", 20));
    command->pushCommandBack(table->assignVariable("c", 30));

    command->pushCommandBack(new DISPLAY_REGISTERS());

    command->pushCommandBack(new COPY_FROM_STACK_TO_REGISTER(VMregister::a));
    command->pushCommandBack(new PUT());
    
    command->pushCommandBack(new HALT());
    command->execute();

	return 0;
}