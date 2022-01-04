#ifndef IF_H
#define IF_H

#include "commands.h"
#include "../ComplexCommand.h"

class IF : public ComplexCommand {
public:
	IF(ComplexCommand* condition, ComplexCommand* thenCommands, ComplexCommand* elseCommands) {
        pushCommandBack(condition);
        
        int thenCommandsLength = thenCommands->getLength();
        // na szczytu stosu mamy condition równe 0 lub 1
        // jeśli szczyt stosu równy jest 1, to zrób skok o jedną instrukcję (idź do następnej, to znaczy to thenCOmmands)
        // w przeciwnym razie zrób na tyle duży sok, żeby trafić do elseCommands
    }
};

#endif