%{
    #include <iostream>
    #include "../inc/Command.h"
    #include "../inc/ComplexCommand.h"
    #include "../inc/VariableDirector.h"
    extern int yylex();
    extern int yyparse();
    int yyerror(std::string);

    VariableDirector* variableDirector;
%}

%union
{
    int number;
    char* text;
    Command* command;
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

%token <text> pidentifier
%token <number> num

%type <command> input
%type <command> program
%type <command> declarations
%type <command> commands
%type <command> command
%type <command> identifier
%type <command> expression
%type <command> value
%%
input: program

program:          TOKEN_VAR declarations TOKEN_BEGIN commands TOKEN_END { ComplexCommand* complexCommand = new ComplexCommand(); complexCommand->pushCommandBack($2); complexCommand->pushCommandBack($4); $$ = complexCommand; }
                | TOKEN_BEGIN commands TOKEN_END { $$ = $2; }

declarations:     declarations TOKEN_COMMA pidentifier { ComplexCommand* complexCommand = new ComplexCommand(); complexCommand->pushCommandBack($1); complexCommand->pushCommandBack(variableDirector->declareVariable($3)); $$ = complexCommand; }
                | declarations TOKEN_COMMA pidentifier TOKEN_LEFT_SQUARE_BRACKET num TOKEN_COLON num TOKEN_RIGHT_SQUARE_BRACKET { ComplexCommand* complexCommand = new ComplexCommand(); complexCommand->pushCommandBack($1); complexCommand->pushCommandBack(variableDirector->declareArray($3, $5, $7)); $$ = complexCommand; }
                | pidentifier { ComplexCommand* complexCommand = new ComplexCommand(); complexCommand->pushCommandBack(variableDirector->declareVariable($1)); $$ = complexCommand; }
                | pidentifier TOKEN_LEFT_SQUARE_BRACKET num TOKEN_COLON num TOKEN_RIGHT_SQUARE_BRACKET { ComplexCommand* complexCommand = new ComplexCommand(); complexCommand->pushCommandBack(variableDirector->declareArray($1, $3, $5)); $$ = complexCommand; }

commands:         commands command { ComplexCommand* complexCommand = new ComplexCommand(); complexCommand->pushCommandBack($1); complexCommand->pushCommandBack($2); $$ = complexCommand; }
                | command

command:          identifier ASSIGN expression TOKEN_SEMICOLON //TODO { ComplexCommand* complexCommand = new ComplexCommand();  complexCommand->pushCommandBack(variableDirector->assignVariable($1, $3));  $$ = complexCommand; }

expression:       value
                | value TOKEN_PLUS value
                | value TOKEN_MINUS value

value:            num
                | identifier

identifier:       pidentifier { $$ = 100; }
                | pidentifier TOKEN_LEFT_SQUARE_BRACKET pidentifier TOKEN_RIGHT_SQUARE_BRACKET
                | pidentifier TOKEN_LEFT_SQUARE_BRACKET num TOKEN_RIGHT_SQUARE_BRACKET
%%

int yyerror(std::string error) {	
    std::cout << error << std::endl;
}

int main() {
    variableDirector = new VariableDirector();
    yyparse();
    return 0;
}