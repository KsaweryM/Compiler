#ifndef DIV_H
#define DIV_H

#include "commands.h"
#include "../ComplexCommand.h"

class DIV_TWO_NUMBERS_FROM_STACK_AND_PUSH_RESULT_ON_STACK : public ComplexCommand {
public:
    DIV_TWO_NUMBERS_FROM_STACK_AND_PUSH_RESULT_ON_STACK() {
        pushCommandBack(new LOAD_FROM_STACK_TO_REGISTER(VMregister::c));
        pushCommandBack(new LOAD_FROM_STACK_TO_REGISTER(VMregister::a));
        
        // a dzielnia
        // c dzielnik
        // d wynik dzielenia
        // b ujemny licznik wewnątrz pętli
        // e licznik wewnątrz pętli
        // f jedynka
        pushCommandBack(new RESET(VMregister::d));
        pushCommandBack(new RESET(VMregister::f));
        pushCommandBack(new INC(VMregister::f));

        // na początku zakładam, że a >= 0 oraz c > 0
        //                                                                          |A|    |B|   |C|     |D|   |E|  |F|
        // while a >= c <==> while a - c >= 0 <==> jump jeśli a - c < 0              a      ?     c       0     ?    1
        pushCommandBack(new SUB(VMregister::c));                              //    a-c     ?     c       0     ?    1 
        pushCommandBack(new JNEG(33));                                        //    a-c     ?     c       0     ?    1 
            pushCommandBack(new ADD(VMregister::c));                          //     a      ?     c       0     ?    1 
            pushCommandBack(new RESET(VMregister::e));                        //     a      ?     c       0     0    1
            pushCommandBack(new SWAP(VMregister::c));                         //     c      ?     a       0     0    1         while c <= a <==> while c - a <= 0 <==> jump jeśli c - a > 0
            pushCommandBack(new SHIFT(VMregister::a));                        //    c*2     ?     a       0     0    q
            pushCommandBack(new SUB(VMregister::c));                          //   c*2-a    ?     a       0     0    1
            pushCommandBack(new JPOS(6));                                     //    c-a     ?     a       0     0    1
                pushCommandBack(new ADD(VMregister::c));                      //     c      ?     a       0     0    1         // problem z wartością a przy wyjściu z pętli
                pushCommandBack(new SHIFT(VMregister::f));                    //    c*2     ?     a       0     0    1
                pushCommandBack(new SWAP(VMregister::c));                     //     a      ?    c*2      0     0    1
                pushCommandBack(new INC(VMregister::e));                      //     a      ?    c*2      0     1    1
                pushCommandBack(new JUMP(-8));                                //     a      ?    c*2      0     1    1    
            pushCommandBack(new ADD(VMregister::c));                          //     c      ?     a       0     0    1
            pushCommandBack(new SWAP(VMregister::c));                         //     a      ?     c       0     0    1
            pushCommandBack(new DEC(VMregister::f));                          //     a      ? c*2^(e+1)   0     e    0          c > a, ale c / 2 <= a oraz c / 2 = b * 2^e
            pushCommandBack(new DEC(VMregister::f));                          //     a      ? c*2^(e+1)   0     e   -1 
            pushCommandBack(new SWAP(VMregister::c));                         // c*2^(e+1)  ?     a       0     e   -1
            pushCommandBack(new SHIFT(VMregister::f));                        //   c*2^e    ?     a       0     e   -1
            pushCommandBack(new INC(VMregister::f));                          //   c*2^e    ?     a       0     e    0
            pushCommandBack(new INC(VMregister::f));                          //   c*2^e    ?     a       0     e    1
            pushCommandBack(new SWAP(VMregister::c));                         //     a      ?   c*2^e     0     e    1
            pushCommandBack(new SUB(VMregister::c));                          //  a-c*2^e   ?   c*2^e     d     e    1
            pushCommandBack(new SWAP(VMregister::d));                         //     d      ?   c*2^e   a-c^2^e e    1
            pushCommandBack(new ADD(VMregister::e));                          //    d+e     ?   c*2^e   a-c^2^e e    1
            pushCommandBack(new SWAP(VMregister::d));                         //  a-c*2^e   ?   c*2^e    d+e    e    1
            pushCommandBack(new RESET(VMregister::b));                        //  a-c*2^e   0   c*2^e    d+e    e    1 
            pushCommandBack(new SWAP(VMregister::b));                         //     0  a-c*2^e c*2^e    d+e    e    1
            pushCommandBack(new SUB(VMregister::e));                          //    -e  a-c*2^e c*2^e    d+e    e    1
            pushCommandBack(new SWAP(VMregister::b));                         //  a-c*2^e  -e   c*2^e    d+e    e    1
            pushCommandBack(new SWAP(VMregister::c));                         //   c*2^e   -e   a-c*2^e  d+e    e    1
            pushCommandBack(new SHIFT(VMregister::b));                        //     c     -e   a-c*2^e  d+e    e    1
            pushCommandBack(new SWAP(VMregister::c));                         //  a-c*2^e  -e     c      d+e    e    1
            pushCommandBack(new JUMP(-33));                                   //  a-c*2^e  -e     c      d+e    e    1
        pushCommandBack(new SAVE_REGISTER_ON_STACK(VMregister::d));
    }
};

#endif