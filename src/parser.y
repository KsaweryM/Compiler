%{
    #include <iostream>
    //#include "../inc/Command.h"

    extern int yylex();
    extern int yyparse();
    int yyerror(std::string);
%}

%union
{
    int number;
    
    char* text;
}

%token <number> VAR
%token <number> BEGIN
%token <number> END
%token <number> ASSIGN
%left  <number> TOKEN_PLUS
%left  <number> TOKEN_MINUS
%token <number> LEFT_SQUARE_BRACKET
%token <number> RIGHT_SQUARE_BRACKET
%token <number> SEMICOLON
%token <number> COLON
%token <number> COMMA
%token <number> ERROR

%token <number> pidentifier
%token <number> num

%type <number> input
%type <number> program
%type <number> declarations
%type <number> commands
%type <number> command
%type <number> identifier
%type <number> expression
%type <number> value
%%
input: program

program:          VAR declarations BEGIN commands END
                | BEGIN commands END

declarations:     declarations COMMA pidentifier
                | declarations COMMA pidentifier LEFT_SQUARE_BRACKET num COLON num RIGHT_SQUARE_BRACKET
                | pidentifier
                | pidentifier LEFT_SQUARE_BRACKET num COLON num RIGHT_SQUARE_BRACKET

commands:         commands command
                | command

command:          identifier ASSIGN expression SEMICOLON

expression:       value
                | value TOKEN_PLUS value
                | value TOKEN_MINUS value

value:            num
                | identifier

identifier:       pidentifier
                | pidentifier LEFT_SQUARE_BRACKET pidentifier RIGHT_SQUARE_BRACKET
                | pidentifier LEFT_SQUARE_BRACKET num RIGHT_SQUARE_BRACKET
%%

int yyerror(std::string error) {	
    std::cout << error << std::endl;
}

int main() {
    yyparse();
    return 0;
}