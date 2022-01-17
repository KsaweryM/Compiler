#ifndef VARIABLE_DIRECTOR_H
#define VARIABLE_DIRECTOR_H

#define VARIABLE_DIRECTOR_DEBUG 0

#include <string>
#include <map>
#include <stdexcept>
#include "VariableContainer.h"
#include "ComplexCommand.h"

#include <iostream>

class VariableDirector {
private:
    long long int stack = 0;
    std::map<std::string, VariableContainer*> variableContainers;

    /*
        Po zadeklarowaniu wszystkich zmiennych należy zapamiętać wartość stosu. 
        Wszystkie zmienne, których adres nie przekracza tej wartości stosu, są modyfikowalne.
        Gdy w pętli for pojawi się iterator (nie może być wcześniej zadeklarowaną zmienną) 

    */
public:
    Command* declareVariable(std::string name) {
        if (variableContainers.count(name) != 0) {
            std::string text = "Zmienna o tej nazwie zostala wczesniej zadeklarowana!";
            throw std::invalid_argument(text);
        }

        if (VARIABLE_DIRECTOR_DEBUG)
            std::cerr << "Zarządca zmiennych stworzył zmienną \"" << name  << "\"" << std::endl; 

        variableContainers[name] = new VariableContainer(stack, 0, 0);
        stack += 1;

        return new INC(VMregister::h);
    }

    Command* declareIterator(std::string name) {
        if (VARIABLE_DIRECTOR_DEBUG)
            std::cerr << "Zarządca zmiennych stworzył iterator \"" << name  << "\"" << std::endl; 

        VariableContainer* container = new VariableContainer(stack, 0, 0);
        container->setAsIterator();

        variableContainers[name] = container;
        stack += 1;

        return new INC(VMregister::h);
    }

    Command* incrementIterator(std::string name) {
        return variableContainers[name]->incrementFirstElement();
    }

    Command* decrementIterator(std::string name) {
        return variableContainers[name]->decrementFirstElement();
    }

    bool isIterator(std::string name) {
        return variableContainers[name]->isIterator();
    }

    Command* declareArray(std::string name, long long int firstIndex, long long int lastIndex) {
        if (variableContainers.count(name) != 0) {
            std::string text = "Tablica o tej nazwie zostala wczesniej zadeklarowana!";
            throw std::invalid_argument(text);
        }

        if (VARIABLE_DIRECTOR_DEBUG)
            std::cerr << "Zarządca zmiennych stworzył tablice \"" << name << "\"" << std::endl;
        variableContainers[name] = new VariableContainer(stack, firstIndex, lastIndex);
        long long int size = lastIndex - firstIndex + 1;

        stack += size;

        return new INCH(size);      
    }

    Command* pushAddressOfVariableOntoStack(std::string name) {
        if (VARIABLE_DIRECTOR_DEBUG)
            std::cerr << "Zarządca zmiennych ustawia adres zmiennej \"" << name << "\" na stos" << std::endl;

        if (variableContainers.count(name) == 0) {
            std::string text = "zmienna \"" + name + "\" nie istnieje!";
            throw std::invalid_argument(text);
        }

        return variableContainers[name]->pushAddressOfVariableOntoStack(0);
    }

    Command* getIndexFromStackAndPushAddressOfVariableFromArrayOntoStack(std::string name) {
    if (VARIABLE_DIRECTOR_DEBUG)
            std::cerr << "Zarządca zmiennych pobiera indeks ze stosu i ustawia adres zmiennej z tablicy o nazwie \"" << name << "\" na stos" << std::endl;

        if (variableContainers.count(name) == 0) {
            std::string text = "tablica \"" + name + "\" nie istnieje!";
            throw std::invalid_argument(text);
        }

        return variableContainers[name]->getIndexFromStackAndPushAddressOfVariableOntoStack();
    }

    Command* pushAddressOfVariableFromArrayOntoStack(std::string name, long long int index) {
        if (VARIABLE_DIRECTOR_DEBUG)
            std::cerr << "Zarządca zmiennych z indeksem w kodzie ustawia adres zmiennej \"" << name << "\" na stos" << std::endl;

        if (variableContainers.count(name) == 0) {
            std::string text = "tablica \"" + name + "\" nie istnieje!";
            throw std::invalid_argument(text);
        }

        return variableContainers[name]->pushAddressOfVariableOntoStack(index);
    }

    Command* pushVariableOntoStackByAddressOfVariableFromStack() {
        ComplexCommand* command = new ComplexCommand();

        command->pushCommandBack(new SAVE_REGISTER_ON_STACK(VMregister::b));
        command->pushCommandBack(new SAVE_REGISTER_ON_STACK(VMregister::a));
        command->pushCommandBack(new DECN(VMregister::h, 2));
        command->pushCommandBack(new LOAD_FROM_STACK_TO_REGISTER(VMregister::b));
        command->pushCommandBack(new LOAD(VMregister::b));
        command->pushCommandBack(new SAVE_REGISTER_ON_STACK(VMregister::a));
        command->pushCommandBack(new INCN(VMregister::h, 2));
        command->pushCommandBack(new LOAD_FROM_STACK_TO_REGISTER(VMregister::a));
        command->pushCommandBack(new LOAD_FROM_STACK_TO_REGISTER(VMregister::b));

        return command;
    }

    Command* assign() {
        ComplexCommand* command = new ComplexCommand();
        
        command->pushCommandBack(new SAVE_REGISTER_ON_STACK(VMregister::b));
        command->pushCommandBack(new SAVE_REGISTER_ON_STACK(VMregister::a));
        command->pushCommandBack(new DECN(VMregister::h, 2));
        command->pushCommandBack(new LOAD_FROM_STACK_TO_REGISTER(VMregister::a));
        command->pushCommandBack(new LOAD_FROM_STACK_TO_REGISTER(VMregister::b));
        command->pushCommandBack(new STORE(VMregister::b));
        command->pushCommandBack(new INCN(VMregister::h, 4));
        command->pushCommandBack(new LOAD_FROM_STACK_TO_REGISTER(VMregister::b));
        command->pushCommandBack(new LOAD_FROM_STACK_TO_REGISTER(VMregister::a));
        command->pushCommandBack(new DECN(VMregister::h, 2));

        return command;
    }

    /*
    Command* assignVariable(std::string name, int value) {
        return variableContainers[name]->assign(0, value);
    }

    Command* assignVariableByValueFromStack(std::string name) {
        return variableContainers[name]->assignByValueFromStack(0);
    }

    Command* assignVariableFromArray(std::string name, int index, int value) {
        return variableContainers[name]->assign(index, value);
    }

    Command* assignVariableFromArrayByValueFromStack(std::string name, int index) {
        return variableContainers[name]->assignByValueFromStack(index);
    }
    

    Command* pushVariableOntoStack(std::string name) {
        return variableContainers[name]->pushOnStack(0);
    }

    Command* pushVariableFromArrayOntoStack(std::string name, int index) {
        return variableContainers[name]->pushOnStack(index);
    }

    */

    ~VariableDirector() {
        std::map<std::string, VariableContainer*>::iterator it;

		for (it = variableContainers.begin(); it != variableContainers.end(); it++) {
			delete it->second;
		}
    }
};

#endif