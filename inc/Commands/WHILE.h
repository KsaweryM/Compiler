#ifndef WHILE_H
#define WHILE_H

#include "commands.h"
#include "../ComplexCommand.h"

// zmienia wartości rejestrów
// brak założeń na początkowy stan rejestrów
// część z klas command niepotrzebnie przywraca poprzedni stan rejestrów
// w procesie optymalizacji można usunąć te przywracania
class WHILE : public ComplexCommand {
public:
	WHILE(Command* condition, Command* commands) {
        /*
        // na szczycie stosu zapisujemy wartość condition
        pushCommandBack(condition);

        // teraz condition, które jest równe 0 lub 1 jest w rejestrze a
        pushCommandBack(new LOAD_FROM_STACK_TO_REGISTER(VMregister::a));

        int commandsLength = commands->getLength();

        ComplexCommand* complex = new ComplexCommand();

        complexThen->pushCommandBack(thenCommands);
        complexThen->pushCommandBack(new JUMP(elseCommandsLength + 1));
        
        int complexThenLength = complexThen->getLength();
      

        // jeśli condition == 0, to przeskocz do else
        pushCommandBack(new JZERO(complexThenLength + 1));
        pushCommandBack(complexThen);
        pushCommandBack(elseCommands);
        */
    }
};

#endif