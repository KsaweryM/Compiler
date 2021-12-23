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

%token <number> TOKEN_VAR
%token <number> TOKEN_BEGIN
%token <number> TOKEN_END
%token <number> ASSIGN
%left  <number> TOKEN_PLUS
%left  <number> TOKEN_MINUS
%token <number> TOKEN_LEFT_SQUARE_BRACKET
%token <number> TOKEN_RIGHT_SQUARE_BRACKET
%token <number> TOKEN_SEMICOLON
%token <number> TOKEN_COLON
%token <number> TOKEN_COMMA
%token <number> TOKEN_ERROR

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

program:          TOKEN_VAR declarations TOKEN_BEGIN commands TOKEN_END
                | TOKEN_BEGIN commands TOKEN_END

declarations:     declarations TOKEN_COMMA pidentifier
                | declarations TOKEN_COMMA pidentifier TOKEN_LEFT_SQUARE_BRACKET num TOKEN_COLON num TOKEN_RIGHT_SQUARE_BRACKET
                | pidentifier
                | pidentifier TOKEN_LEFT_SQUARE_BRACKET num TOKEN_COLON num TOKEN_RIGHT_SQUARE_BRACKET

commands:         commands command
                | command

command:          identifier ASSIGN expression TOKEN_SEMICOLON

expression:       value
                | value TOKEN_PLUS value
                | value TOKEN_MINUS value

value:            num
                | identifier

identifier:       pidentifier
                | pidentifier TOKEN_LEFT_SQUARE_BRACKET pidentifier TOKEN_RIGHT_SQUARE_BRACKET
                | pidentifier TOKEN_LEFT_SQUARE_BRACKET num TOKEN_RIGHT_SQUARE_BRACKET
%%

int yyerror(std::string error) {	
    std::cout << error << std::endl;
}

int main() {
    yyparse();
    return 0;
}