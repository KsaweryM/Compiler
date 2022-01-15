#ifndef TIMES_H
#define TIMES_H

#include "commands.h"
#include "../ComplexCommand.h"

class TIMES_TWO_NUMBERS_FROM_STACK_AND_PUSH_RESULT_ON_STACK : public ComplexCommand {
public:
	TIMES_TWO_NUMBERS_FROM_STACK_AND_PUSH_RESULT_ON_STACK() {
      pushCommandBack(new LOAD_FROM_STACK_TO_REGISTER(VMregister::a));       // liczymy A * B, gdzie w rejestrze a jest wartość A i w rejestrze B jest wartość b
      pushCommandBack(new LOAD_FROM_STACK_TO_REGISTER(VMregister::b));
      pushCommandBack(new RESET(VMregister::c));       // rejestr, w którym będzie wynik
      pushCommandBack(new RESET(VMregister::f));       // rejestr, zawierający stałą 1
      pushCommandBack(new INC(VMregister::f));
      pushCommandBack(new SWAP(VMregister::b));       // zakładamy, że przed wykonaniem tej operacji w rejestrze a była wartość A. 
      pushCommandBack(new JZERO(31));                 // while b > 0
        pushCommandBack(new SWAP(VMregister::b));     // po wykonaniu tej operacji w rejestrze a będzie znowu wartość A, a w rejestrze b będzie znowu wartość B
        pushCommandBack(new RESET(VMregister::d));    // szukamy największej potęgi dwójki, które będzie mniejsza od b 
        pushCommandBack(new INC(VMregister::d));      // w tym celu zaczynając od 2^(e + 1), e = 1, i szukamy najmniejszej liczby e, takiej że 2^e > b
        pushCommandBack(new INC(VMregister::d));      // wtedy 2^(e + 1) / 2 = 2^e da nam największą potęgę dwójki, która jest mniejsza od b
        pushCommandBack(new RESET(VMregister::e));
        pushCommandBack(new RESET(VMregister::g));    // g = -e
        pushCommandBack(new SWAP(VMregister::d));     // teraz w rejestrze a będzie wartość rejestru d
        pushCommandBack(new SUB(VMregister::b));      // w rejestrze a mamy wartość d - b
        pushCommandBack(new JPOS(6));                 // while d <= b <===> d - b <= 0. Czyli wychodzimy z pętli, jeśli d - b > 0  
            pushCommandBack(new ADD(VMregister::b));  // wchodzimy do pętli, teraz a = d - b + b = d
            pushCommandBack(new SHIFT(VMregister::f)); // a = d * 2
            pushCommandBack(new INC(VMregister::e));   // e++
            pushCommandBack(new DEC(VMregister::g));   // g--
            pushCommandBack(new JUMP(-6));
        pushCommandBack(new ADD(VMregister::b));       // wyszliśmy z pętli, w rejestrze a mamy d - b + b = d
        pushCommandBack(new DEC(VMregister::f));       // obecnie f = 0, chcemy podzielić d przez 2, więc musimy zrobić shift(-1)       
        pushCommandBack(new DEC(VMregister::f));       // teraz f = -1
        pushCommandBack(new SHIFT(VMregister::f));     // teraz w rejestrze a mamy d / 2, i jest to największa liczba 2^k, która jest mniejsza od b
        pushCommandBack(new INC(VMregister::f)); 
        pushCommandBack(new INC(VMregister::f));       // f wróciło do pierwotniej wartości 1
        pushCommandBack(new SWAP(VMregister::d));      // teraz w rejestrze d mamy wartość d, a w rejestrze a mamy wartość a
        pushCommandBack(new SWAP(VMregister::b));      // teraz w rejestrze b mamy wartość a, a w rejestrze a mamy wartość b
        pushCommandBack(new SUB(VMregister::d));       // teraz w rejestrze a mamy wartość b - d
        pushCommandBack(new SWAP(VMregister::b));      // teraz w rejestrze a mamy wartość a, a w rejestrze b mamy wartość b - d
        pushCommandBack(new SHIFT(VMregister::e));     // teraz w rejestrze a mamy wartość a * 2^e
        pushCommandBack(new SWAP(VMregister::c));      // teraz w rejestrze a mamy wartość c (obecny wynik mnożenia), a w rejestrze c mamy a * 2^e
        pushCommandBack(new ADD(VMregister::c));       // teraz w rejestrze a mamy wartość c + a * 2^e
        pushCommandBack(new SWAP(VMregister::c));      // teraz w rejestrze a mamy wartość a * 2^e, a w rejestrze c mamy wartość c + a*2^e
        pushCommandBack(new SHIFT(VMregister::g));     // a wraca do pierwotnej wartości, g == -e, więc w rejestrze a mamy a * 2^e * 2^(-e) = a, a w rejestrze c obecny wynik mnożenia
        pushCommandBack(new JUMP(-31));                // 
      pushCommandBack(new SAVE_REGISTER_ON_STACK(VMregister::c)); 
    }
};

#endif