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
        pushCommandBack(new RESET(VMregister::g));
        pushCommandBack(new SWAP(VMregister::g));
        pushCommandBack(new ADD(VMregister::c));
        pushCommandBack(new SWAP(VMregister::g));

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
        //                                                               |a| % |c|          ?              c           [|a|/|c|]         ?              a
        //
        pushCommandBack(new RESET(VMregister::b));
        pushCommandBack(new RESET(VMregister::e));
        //                                                               |a| % |c|          0               c          [|a|/|c|]          0              a
        pushCommandBack(new SWAP(VMregister::b));
        //                                                                   0          |a| % |C|           c          [|a|/|c|]          0              a
        pushCommandBack(new SUB(VMregister::c));
        //                                                                  -c          |a| % |C|           c          [|a|/|c|]          0              a
        pushCommandBack(new SWAP(VMregister::b));
        //                                                               |a| % |c|          -c              c          [|a|/|c|]          0              a
        pushCommandBack(new SWAP(VMregister::e));
        //                                                                   0              -c              c          [|a|/|c|]      |a| % |B|          a
        pushCommandBack(new SUB(VMregister::f));
        //                                                                  -a              -c              c          [|a|/|c|]      |a| % |B|          a
        //pushCommandBack(new RESET(VMregister::g));
        //                                                                  -a              -c              c          [|a|/|c|]      |a| % |B|          a              C       

        // błąd polega na tym, że zapamiętujemy wartość |c| i uznajemy, że dzielnik był nieujemny
        // można zapisać oryginalną wartość c do rejestru g i jeżeli wartość tego rejestru jest mniejesza od 0, to zamień miejscami rejestry b i c

        pushCommandBack(new SWAP(VMregister::g));
        //                                                                   C              -c              c          [|a|/|c|]      |a| % |B|          a             -a       
        pushCommandBack(new JPOS(4));
        pushCommandBack(new SWAP(VMregister::b));
        //                                                                  -c               C              c          [|a|/|c|]      |a| % |B|          a             -a       
        pushCommandBack(new SWAP(VMregister::c));
        //                                                                   c               C              -c         [|a|/|c|]      |a| % |B|          a             -a       
        pushCommandBack(new SWAP(VMregister::b));
        //                                                                   C               c              -c         [|a|/|c|]      |a| % |B|          a             -a       
        pushCommandBack(new SWAP(VMregister::g));
        //                                                                  -a               c              -c         [|a|/|c|]      |a| % |B|          a              C       

        //pushCommandBack(new DISPLAY_REGISTERS());
        //pushCommandBack(new HALT());

        // tutaj zaczynam obliczać wynik całkowitoliczbowy
        pushCommandBack(new RESET(VMregister::g));
        pushCommandBack(new SWAP(VMregister::f));
        //                                                                   a              -c              c          [|a|/|c|]      |a| % |B|          -a             0
        pushCommandBack(new JNEG(16)); 
        // a > 0
        pushCommandBack(new SWAP(VMregister::c));
        //                                                                   c              -c              a          [|a|/|c|]      |a| % |B|          -a             0    
        pushCommandBack(new JNEG(2));
        //      a > 0 and c > 0
        //      jeśli a > 0 i c > 0, to można już skoczyć do rozwiazania
        pushCommandBack(new JUMP(12));
        // JUMP HERE IF a > 0 and c < 0
        //                                                                   c              -c              a          [|a|/|c|]      |a| % |B|          -a             0
        pushCommandBack(new SWAP(VMregister::g));        
        //                                                                   0              -c              a          [|a|/|c|]      |a| % |B|          -a             c
        pushCommandBack(new SUB(VMregister::d));        
        //                                                              -[|a|/|c|]          -c              a          [|a|/|c|]      |a| % |B|          -a             c
        pushCommandBack(new SWAP(VMregister::e));        
        //                                                               |a| % |B|          -c              a          [|a|/|c|]     -[|a|/|c|]          -a             c
        pushCommandBack(new JZERO(2));       
        pushCommandBack(new DEC(VMregister::e));
        //                                                               |a| % |B|          -c              a          [|a|/|c|]     -[|a|/|c|] - 1      -a             c
        pushCommandBack(new SWAP(VMregister::e));
        //                                                              -[|a|/|c|] - 1      -c              a          [|a|/|c|]       |a| % |B|         -a             c
        pushCommandBack(new SWAP(VMregister::d));
        //                                                               [|a|/|c|]          -c              a         -[|a|/|c|] - 1   |a| % |B|         -a             c
        // dla a > 0 and c < 0
        // a % b = |a| % |c| + (|a| % |c| > 0) * (-|b|)
        pushCommandBack(new SWAP(VMregister::e)); 
        //                                                               |a| % |c|          -c              a         -[|a|/|c|] - 1   [|a|/|c|]         -a             c
        pushCommandBack(new JZERO(2));
        pushCommandBack(new SUB(VMregister::b));
        //                                                             |a| % |c| - |c|      -c              a         -[|a|/|c|] - 1   [|a|/|c|]         -a             c
        // w rejestrze a mamy "a % c", w w rejestrze d mamy [a/b]
        pushCommandBack(new SWAP(VMregister::e));

        //                                                               [|a|/|c|]          -c              a         -[|a|/|c|] - 1  |a| % |c| - |c|    -a             c
        pushCommandBack(new JUMP(12)); // zapisz wartość rejestru
        // JUMP HERE IF a < 0
        //                                                                   a              -c              c          [|a|/|c|]      |a| % |B|          -a             0
        pushCommandBack(new SWAP(VMregister::c));
        //                                                                   c              -c              a          [|a|/|c|]      |a| % |B|          -a             0
        
        pushCommandBack(new JNEG(11)); // wskocz do przypadku a < 0 i b < 0
        // a < 0 and b > 0
        //                                                                   c              -c              a          [|a|/|c|]      |a| % |B|          -a             0
        pushCommandBack(new SWAP(VMregister::g));
        //                                                                   0              -c              a          [|a|/|c|]      |a| % |B|          -a             c
        pushCommandBack(new SUB(VMregister::d));
        //                                                              -[|a|/|c|]          -c              a          [|a|/|c|]      |a| % |B|          -a             c
        pushCommandBack(new SWAP(VMregister::d));
        //                                                               [|a|/|c|]          -c              a         -[|a|/|c|]      |a| % |B|          -a             c
        pushCommandBack(new RESET(VMregister::a));
        //                                                                   0              -c              a         -[|a|/|c|]      |a| % |B|          -a             c
        pushCommandBack(new SUB(VMregister::e));
        //                                                              -|a| % |b|          -c              a         -[|a|/|c|]      |a| % |B|          -a             c
        pushCommandBack(new JZERO(4));
        pushCommandBack(new ADD(VMregister::g));
        //                                                            -|a| % |b| + |b|      -c              a         -[|a|/|c|]      |a| % |B|          -a             c
        pushCommandBack(new SWAP(VMregister::e));
        //                                                               |a| % |b|          -c              a         -[|a|/|c|]    -|a| % |B| + |b|      -a             c
        pushCommandBack(new DEC(VMregister::d));
        //                                                               |a| % |b|          -c              a      (-[|a|/|c|] - 1) -|a| % |B| + |b|      -a             c
        pushCommandBack(new JUMP(4)); // wskocz do save register on stack d
        // a < 0 and b < 0
        //                                                                   c              -c              a          [|a|/|c|]      |a| % |B|          -a             0
        pushCommandBack(new SWAP(VMregister::g));
        //                                                                   0              -c              a          [|a|/|c|]      |a| % |B|          -a             c
        pushCommandBack(new SUB(VMregister::e));
        //                                                             - |a| % |b|          -c              a          [|a|/|c|]      |a| % |B|          -a             c
        pushCommandBack(new SWAP(VMregister::e));
        //                                                               |a| % |b|          -c              a          [|a|/|c|]    - |a| % |B|          -a             c

        pushCommandBack(new SAVE_REGISTER_ON_STACK(VMregister::d));
    }
};

#endif