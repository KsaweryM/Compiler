#ifndef IF_H
#define IF_H

#include "commands.h"
#include "../ComplexCommand.h"

// zmienia wartości rejestrów
// brak założeń na początkowy stan rejestrów
// część z klas command niepotrzebnie przywraca poprzedni stan rejestrów
// w procesie optymalizacji można usunąć te przywracania
class IF : public ComplexCommand {
public:
	IF(Command* condition, Command* thenCommands, Command* elseCommands) {
        
        // na szczycie stosu zapisujemy wartość condition
        pushCommandBack(condition);

        // teraz condition, które jest równe 0 lub 1 jest w rejestrze a
        pushCommandBack(new LOAD_FROM_STACK_TO_REGISTER(VMregister::a));

        long long int elseCommandsLength = elseCommands->getLength();

        ComplexCommand* complexThen = new ComplexCommand();

        complexThen->pushCommandBack(thenCommands);
        complexThen->pushCommandBack(new JUMP(elseCommandsLength + 1));
        
        long long int complexThenLength = complexThen->getLength();
      

        // jeśli condition == 0, to przeskocz do else
        pushCommandBack(new JZERO(complexThenLength + 1));
        pushCommandBack(complexThen);
        pushCommandBack(elseCommands);
    }
};

#endif