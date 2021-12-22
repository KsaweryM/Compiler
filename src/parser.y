%{
    #include <iostream>
    #include "../inc/Commands/commands.h"
    #include "../inc/VMregister.h"

    extern int yylex();
    extern int yyparse();
    int yyerror(std::string);
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
    | input line { $$ = new LOAD_FROM_STACK_TO_REGISTER(VMregister::a); $$->execute(); $$ = new PUT(); $$->execute(); }
;
line: exp TOKEN_END {        }
    | TOKEN_END {    }
;

exp: TOKEN_NUMBER                { $$ = new PUSH($1); $$->execute(); }
    | exp TOKEN_PLUS exp         { $$ = new ADD_TWO_NUMBERS_FROM_STACK_AND_PUSH_RESULT_ON_STACK(); $$->execute();   }
    | exp TOKEN_MINUS exp        { $$ = new SUBTRACT_TWO_NUMBERS_FROM_STACK_AND_PUSH_RESULT_ON_STACK(); $$->execute();   }
    | exp TOKEN_MULTI exp        {  }
    | TOKEN_LNAW exp TOKEN_PNAW  { $$ = $1; }
    | TOKEN_MINUS exp %prec TOKEN_NEG { $$ = new CHANGE_SIGN_ON_STACK(); $$->execute();  }
%%

int yyerror(std::string error) {	
    std::cout << error << std::endl;
}

int main() {
    yyparse();
    HALT* halt = new HALT();
    halt->execute(); 
    return 0;
}