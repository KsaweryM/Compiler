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

        // f <==> kopia a

        pushCommandBack(new RESET(VMregister::f));            
        pushCommandBack(new SWAP(VMregister::f));
        pushCommandBack(new ADD(VMregister::f));

        pushCommandBack(new JPOS(5));
        pushCommandBack(new JZERO(4));
        pushCommandBack(new RESET(VMregister::b));
        pushCommandBack(new SWAP(VMregister::b)); // teraz w rejestrze a jest wartość 0, a w rejestrze b jest wartość a
        pushCommandBack(new SUB(VMregister::b));  // teraz w rejestrze a jest wartość -a, a w rejestrze b jest wartość a

        pushCommandBack(new SWAP(VMregister::c));
        
        pushCommandBack(new JPOS(5));
        pushCommandBack(new JZERO(4));
        pushCommandBack(new RESET(VMregister::b));
        pushCommandBack(new SWAP(VMregister::b));
        pushCommandBack(new SUB(VMregister::b));

        pushCommandBack(new SWAP(VMregister::c));

        pushCommandBack(new RESET(VMregister::d));
        
        // dzielenie przez zero daje zero
        pushCommandBack(new SWAP(VMregister::c));
        pushCommandBack(new JZERO(38));
        pushCommandBack(new SWAP(VMregister::c));

        // na początku zakładam, że a >= 0 oraz c > 0
        //                                                                              |A|       |B|       |C|       |D|       |E|     
                                                                                //       a         ?         c         0         ?       
        pushCommandBack(new SUB(VMregister::c));                                //      a-c        ?         c         0         ?          // wracając do pętli w rejestrze a musi być wartość a, w rejestrze c musi być wartość c
            pushCommandBack(new JNEG(35));                                      //      a-c        ?         c         0         ?       
            pushCommandBack(new RESET(VMregister::e));                          //      a-c        ?         c         0         0       
            pushCommandBack(new ADD(VMregister::c));                            //       a         ?         c         0         0       
            pushCommandBack(new SUB(VMregister::c));                            //      a-c        ?         c         0         0         // wracając do pętli w rejestrze a musi być wartość a, w rejestrze c musi być wartość c
                pushCommandBack(new JNEG(9));                                   //      a-c        ?         c         0         0       
                pushCommandBack(new ADD(VMregister::c));                        //       a         ?         c         0         0       
                pushCommandBack(new INC(VMregister::e));                        //       a         ?         c         0         1       
                pushCommandBack(new SWAP(VMregister::c));                       //       c         ?         a         0         1       
                pushCommandBack(new RESET(VMregister::b));                      //       c         0         a         0         1       
                pushCommandBack(new INC(VMregister::b));                        //       c         1         a         0         1       
                pushCommandBack(new SHIFT(VMregister::b));                      //      c*2        ?         a         0         1       
                pushCommandBack(new SWAP(VMregister::c));                       //       a         ?        c*2        0         1       
                pushCommandBack(new JUMP(-9));                                  // powrót od condition pętli
                pushCommandBack(new ADD(VMregister::c));                        // wyjście z pęlti, powrót do wartości rejestru a przed condition
            pushCommandBack(new DEC(VMregister::e));                            //       a         ?       c*2^e       0        e-1      
            pushCommandBack(new RESET(VMregister::b));                          //       a         0       c*2^e       0        e-1      
            pushCommandBack(new DEC(VMregister::b));                            //       a        -1       c*2^e       0        e-1      
            pushCommandBack(new SWAP(VMregister::c));                           //      c*2^e     -1         a         0        e-1      
            pushCommandBack(new SHIFT(VMregister::b));                          //    c*2^(e-1)   -1         a         0        e-1      
            pushCommandBack(new SWAP(VMregister::c));                           //       a        -1      c*2^(e-1)    0        e-1      
            pushCommandBack(new SUB(VMregister::c));                            //  a-c*2^(e-1)   -1      c*2^(e-1)    0        e-1      
            pushCommandBack(new SWAP(VMregister::b));                           //      -1   a-c*2^(e-1)  c*2^(e-1)    0        e-1      
            pushCommandBack(new INC(VMregister::a));                            //       0   a-c*2^(e-1)  c*2^(e-1)    0        e-1      
            pushCommandBack(new INC(VMregister::a));                            //       1   a-c*2^(e-1)  c*2^(e-1)    0        e-1       
            pushCommandBack(new SHIFT(VMregister::e));                          //  2^(e-1)  a-c*2^(e-1)  c*2^(e-1)    d        e-1      
            pushCommandBack(new SWAP(VMregister::d));                           //       d   a-c*2^(e-1)  c*2^(e-1)  2^(e-1)    e-1      
            pushCommandBack(new ADD(VMregister::d));                            // d+2^(e-1) a-c*2^(e-1)  c*2^(e-1)  2^(e-1)    e-1      
            pushCommandBack(new SWAP(VMregister::d));                           //   2^(e-1) a-c*2^(e-1)  c*2^(e-1)  d+2^(e-1)  e-1      
            pushCommandBack(new RESET(VMregister::a));                          //       0   a-c*2^(e-1)  c*2^(e-1)  d+2^(e-1)  e-1      
            pushCommandBack(new SUB(VMregister::e));                            //    -(e-1) a-c*2^(e-1)  c*2^(e-1)  d+2^(e-1)  e-1      
            pushCommandBack(new SWAP(VMregister::c));                           // c*2^(e-1) a-c*2^(e-1)  -(e-1)     d+2^(e-1)  e-1      
            pushCommandBack(new SHIFT(VMregister::c));                          //       c   a-c*2^(e-1)  -(e-1)     d+2^(e-1)  e-1      
            pushCommandBack(new SWAP(VMregister::c));                           //   -(e-1)  a-c*2^(e-1)     c       d+2^(e-1)  e-1      
            pushCommandBack(new SWAP(VMregister::b));                           // a-c*2^(e-1) -(e-1)        c       d+2^(e-1)  e-1      
            pushCommandBack(new JUMP(-35));


        pushCommandBack(new ADD(VMregister::c));


        // powyżej zostało wykonane dzielenie |a| i |c| ( dla wartości bezwzględnych )
        // w rejestrze a znajduje się |a| % |c|
        // w rejestrze d znajduje się [|a|/|c|]
        // f == 1 <==> a < 0
        // g == 1 <==> c < 0

        //                                                                  |A|            |B|            |C|            |D|            |E|            |F|            |G|
        // a > 0 <==> f == 0                                             |a| % |c|                         c          [|a|/|c|]                         a
        // if a > 0
        pushCommandBack(new SWAP(VMregister::f));                                                   
        pushCommandBack(new PUT());
        







        pushCommandBack(new SAVE_REGISTER_ON_STACK(VMregister::d));
    }
};

#endif