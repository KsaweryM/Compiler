#ifndef IF_THEN_H
#define IF_THEN_H

#include "commands.h"
#include "../ComplexCommand.h"

// zmienia wartości rejestrów
// brak założeń na początkowy stan rejestrów
// część z klas command niepotrzebnie przywraca poprzedni stan rejestrów
// w procesie optymalizacji można usunąć te przywracania
class IF_THEN : public ComplexCommand {
public:
	IF_THEN(Command* condition, Command* thenCommands) {
        
        // na szczycie stosu zapisujemy wartość condition
        pushCommandBack(condition);

        // teraz condition, które jest równe 0 lub 1 jest w rejestrze a
        pushCommandBack(new LOAD_FROM_STACK_TO_REGISTER(VMregister::a));

        // jeśli condition == 0, to przeskocz complexThen
        pushCommandBack(new JZERO(thenCommands->getLength() + 1));
        pushCommandBack(thenCommands);
    }
};

#endif