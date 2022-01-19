#ifndef VARIABLE_CONTAINER_H
#define VARIABLE_CONTAINER_H

#include "Variable.h"
#include <vector>
#include "Commands/commands.h"
#include <set>

class VariableContainer {
private:
    long long int containerAddress;
    long long int firstIndex;
    long long int lastIndex;
    long long int size;
    bool iterator = false;
    bool isThisArray = false;
    std::set<int> initializedVariables;

public:
    VariableContainer(long long int address, long long int firstIndex, long long int lastIndex) {
        this->firstIndex = firstIndex;
        this->lastIndex = lastIndex;
        this->size = lastIndex - firstIndex + 1;
        this->containerAddress = address;

        if (firstIndex > lastIndex) {
            std::string text = "Pierwszy indeks tablicy jest wiekszy od drugiego!";
            throw std::invalid_argument(text);
        }
    }

    void initializeVariable(int index) {
        if (index < firstIndex || index > lastIndex) {
            std::string text = "Niepoprawny indeks!";
            throw std::invalid_argument(text);
        }

        initializedVariables.insert(index);
    }

    bool isUninitialized(int index) {
        if (index < firstIndex || index > lastIndex) {
            std::string text = "Niepoprawny indeks!";
            throw std::invalid_argument(text);
        }

        return initializedVariables.count(index) == 0;
    }

    void setAsIterator() {
        iterator = true;
        isThisArray = false;
    }

    void setAsArray() {
        isThisArray = true;
    }

    void setAsVariable() {
        isThisArray = false;
    }

    bool isArray() {
        return isThisArray;
    }

    bool isVariable() {
        return !isThisArray;
    }

    bool isIterator() {
        return iterator;
    }

    ComplexCommand* incrementFirstElement() {
        ComplexCommand* command = new ComplexCommand();
        
        command->pushCommandBack(new PUSH(containerAddress));
        command->pushCommandBack(new LOAD_FROM_STACK_TO_REGISTER(VMregister::b));
        command->pushCommandBack(new LOAD(VMregister::b));
        command->pushCommandBack(new INC(VMregister::a));
        command->pushCommandBack(new STORE(VMregister::b));

        return command;
    }

    ComplexCommand* decrementFirstElement() {
        ComplexCommand* command = new ComplexCommand();
        
        command->pushCommandBack(new PUSH(containerAddress));
        command->pushCommandBack(new LOAD_FROM_STACK_TO_REGISTER(VMregister::b));
        command->pushCommandBack(new LOAD(VMregister::b));
        command->pushCommandBack(new DEC(VMregister::a));
        command->pushCommandBack(new STORE(VMregister::b));

        return command;
    }

    ComplexCommand* pushAddressOfVariableOntoStack(long long int index) {
        if (index < firstIndex || index > lastIndex) {
            std::cerr << "index = " << index << " firstIndex = " << firstIndex << " lastIndex = " << lastIndex << std::endl;
            throw std::invalid_argument("Niepoprawny indeks");
        }

        return new PUSH(containerAddress + (index - firstIndex));
        //return variables[index - firstIndex]->pushAddressOntoStack();
    }

    ComplexCommand* getIndexFromStackAndPushAddressOfVariableOntoStack() {
        ComplexCommand* complex = new ComplexCommand();
        
        // na szczycie stosu mamy adres indeksu tablicy 
        // zapisujemy na szczyt stosu adres tablicy oraz wartość fist indeks
        complex->pushCommandBack(new PUSH(containerAddress));
        complex->pushCommandBack(new PUSH(firstIndex));

        // staramy się uzyskąć [adres tablicy] + ([index tablicy] - [firstIndex]) 

        // zapisujemy rejestry a, b, c, d na stosie
        complex->pushCommandBack(new SAVE_REGISTER_ON_STACK(VMregister::a));
        complex->pushCommandBack(new SAVE_REGISTER_ON_STACK(VMregister::b));
        complex->pushCommandBack(new SAVE_REGISTER_ON_STACK(VMregister::c));
        complex->pushCommandBack(new SAVE_REGISTER_ON_STACK(VMregister::d));

        // szczyt stosu ustawiony jest w miejsce firstIndex
        complex->pushCommandBack(new DECN(VMregister::h, 4));
        
        // rejestr b zawiera wartość firstIndex
        complex->pushCommandBack(new LOAD_FROM_STACK_TO_REGISTER(VMregister::b));

        // adres tablicy jest w rejestrze c
        complex->pushCommandBack(new LOAD_FROM_STACK_TO_REGISTER(VMregister::c));

        // adres indeksu jest w rejestrze d
        complex->pushCommandBack(new LOAD_FROM_STACK_TO_REGISTER(VMregister::d));

        // wartość indeksu jest w rejestrze a
        complex->pushCommandBack(new LOAD(VMregister::d));

        // wartość indeksu jest w rejestrze d
        complex->pushCommandBack(new SWAP(VMregister::d));


        // więc c zawiera adres tablicy, b zawiera firstIndex, d zawiera index
         // staramy się uzyskąć [adres tablicy] + ([index tablicy] - [firstIndex]) 
        // więc na stosie zapisujemy a, gdzie a = c + (d - b)
        // RESET a
        // ADD c
        // ADD d
        // SUB b

        complex->pushCommandBack(new RESET(VMregister::a));
        complex->pushCommandBack(new ADD(VMregister::c));
        complex->pushCommandBack(new ADD(VMregister::d));
        complex->pushCommandBack(new SUB(VMregister::b));

        // w rejestrze a mamy szukany adres. Należy wstawić go w miejsce adresu indeksu oraz przywrócić stare wartości rejestrów.

        complex->pushCommandBack(new SAVE_REGISTER_ON_STACK(VMregister::a));

        // przywracamy stare wartości rejestrów
        
        complex->pushCommandBack(new INCN(VMregister::h, 6));

        complex->pushCommandBack(new LOAD_FROM_STACK_TO_REGISTER(VMregister::d));
        complex->pushCommandBack(new LOAD_FROM_STACK_TO_REGISTER(VMregister::c));
        complex->pushCommandBack(new LOAD_FROM_STACK_TO_REGISTER(VMregister::b));
        complex->pushCommandBack(new LOAD_FROM_STACK_TO_REGISTER(VMregister::a));

        complex->pushCommandBack(new DECN(VMregister::h, 2));
        
        return complex;
    }

    /*
    ComplexCommand* assign(int index, int value) {
        return variables[index - firstIndex]->assign(value);
    }

    ComplexCommand* assignByValueFromStack(int index) {
        return variables[index - firstIndex]->assignByValueFromStack();
    }

    ComplexCommand* pushOnStack(int index) {
        return variables[index - firstIndex]->pushOnStack();
    }
    */

    ~VariableContainer() {
        /*
        std::vector<Variable*>::iterator it;

        for (it = variables.begin(); it != variables.end(); it++) {
            delete* it;
        }
        */
    }
};

#endif