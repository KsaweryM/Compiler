%{
    #include <iostream>
    #include "../inc/Commands/commands.h"
    #include "../inc/VMregister.h"

    extern int yylex();
    extern int yyparse();
    int yyerror(std::string);

    Command* push(int n) {
        return new PUSH(n);
    }

    Command* add(Command* exp1, Command* exp2) {
        ADD_TWO_NUMBERS_FROM_STACK_AND_PUSH_RESULT_ON_STACK* ADD_NUMBERS = new ADD_TWO_NUMBERS_FROM_STACK_AND_PUSH_RESULT_ON_STACK();
        
        ADD_NUMBERS->pushCommandFront(exp1);
        ADD_NUMBERS->pushCommandFront(exp2);
        return ADD_NUMBERS;
    }

    Command* subtract(Command* exp1, Command* exp2) {
        SUBTRACT_TWO_NUMBERS_FROM_STACK_AND_PUSH_RESULT_ON_STACK* SUBTRACT_NUMBERS = new SUBTRACT_TWO_NUMBERS_FROM_STACK_AND_PUSH_RESULT_ON_STACK();

        SUBTRACT_NUMBERS->pushCommandFront(exp1);
        SUBTRACT_NUMBERS->pushCommandFront(exp2);

        return SUBTRACT_NUMBERS;
    }

    Command* changeSign(Command* exp) {
        CHANGE_SIGN_ON_STACK* CHANGE_SIGN = new CHANGE_SIGN_ON_STACK();
        
        CHANGE_SIGN->pushCommandFront(exp);

        return CHANGE_SIGN;
    }

    void createLine(Command* command) {
        ComplexCommand* complex = new ComplexCommand();

        complex->pushCommandBack(command);
        complex->pushCommandBack(new LOAD_FROM_STACK_TO_REGISTER(VMregister::a));
        complex->pushCommandBack(new PUT());

        complex->execute();

        delete complex;
    }
%}
%union
{
    int number;
    Command* cmd;
}

%token <number> TOKEN_NUMBER

%left <cmd> TOKEN_PLUS
%left <cmd> TOKEN_MINUS
%left <cmd> TOKEN_MULTI
%left <cmd> TOKEN_DIV
%token <cmd> TOKEN_LNAW
%token <cmd> TOKEN_PNAW
%token <cmd> TOKEN_ERROR
%token <cmd> TOKEN_END
%token <cmd> TOKEN_FINISH
%precedence <cmd> TOKEN_NEG

%type<cmd> input
%type<cmd> line
%type<cmd> exp

%%
input: %empty {  }
    | input line {  }
;
line: exp TOKEN_END {  createLine($1);       }
    | TOKEN_END {    }
;

exp: TOKEN_NUMBER                { $$ = push($1); }
    | exp TOKEN_PLUS exp         { $$ = add($1, $3);  }
    | exp TOKEN_MINUS exp        { $$ = subtract($1, $3);  }
    | exp TOKEN_MULTI exp        {  }
    | TOKEN_LNAW exp TOKEN_PNAW 
    | TOKEN_MINUS exp %prec TOKEN_NEG { $$ = changeSign($1);  }
%%

int yyerror(std::string error) {	
    std::cout << error << std::endl;
}

int main() {
    yyparse();
    HALT* halt = new HALT();
    halt->execute(); 
    delete halt;
    return 0;
}