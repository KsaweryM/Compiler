%{
    #include <iostream>
    #include "../inc/Commands/commands.h"
    #include "../inc/VMregister.h"
    #include "../inc/ParserCommands/commands.h"
    #include "../inc/DataTable.h"

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