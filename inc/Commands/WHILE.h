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

        ComplexCommand* ifBody = new ComplexCommand();
        LOAD_FROM_STACK_TO_REGISTER* load = new LOAD_FROM_STACK_TO_REGISTER(VMregister::a);
        ifBody->pushCommandBack(condition); // na początku na stos ustawiamy zmienną condition
        ifBody->pushCommandBack(load); // pobieramy ją z stosu
        // jeżeli condition == 0, to przeskocz poniższe komendy, komende odpowiedzialną za powrót do ifBody
        //ifBody->pushCommandBack(new DISPLAY_REGISTER(VMregister::h));
        //ifBody->pushCommandBack(new DISPLAY_REGISTER(VMregister::a));
        ifBody->pushCommandBack(new JZERO(commands->getLength() + 2)); 

        // poniższe zmienna 
        int distanceToIf = condition->getLength() + load->getLength() + commands->getLength() + 1;
        ComplexCommand* loopBody = new ComplexCommand();
        loopBody->pushCommandBack(commands);
        loopBody->pushCommandBack(new JUMP(-distanceToIf));

        pushCommandBack(ifBody);
        pushCommandBack(loopBody);

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