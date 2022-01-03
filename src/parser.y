%{
    #include <iostream>
    #include "../inc/Command.h"
    #include "../inc/ComplexCommand.h"
    #include "../inc/VariableDirector.h"
    #include "../inc/Commands/commands.h"
    extern int yylex();
    extern int yyparse();
    int yyerror(std::string);

    VariableDirector* variableDirector;

    #define PARSER_DEBUG 0
    #define DISPLAY_STACK_END 1
%}

%union
{
    int number;
    char* text;
    Command* com;
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

%type <com> input
%type <com> program
%type <com> declarations
%type <com> commands
%type <com> command
%type <com> identifier
%type <com> expression
%type <com> value
%%
input: program

program:          TOKEN_VAR declarations TOKEN_BEGIN commands TOKEN_END  {if(PARSER_DEBUG) std::cerr << "Tworzę program" << std::endl; ComplexCommand* complexCommand = new ComplexCommand(); complexCommand->pushCommandBack(new RESET_STACK()); complexCommand->pushCommandBack($2); complexCommand->pushCommandBack($4); if (DISPLAY_STACK_END) complexCommand->pushCommandBack(new DISPLAY_STACK_N(10));  complexCommand->pushCommandBack(new HALT());   complexCommand->execute(); $$ = complexCommand; }
                | TOKEN_BEGIN commands TOKEN_END { $$ = $2; }

declarations:     declarations TOKEN_COMMA pidentifier { if(PARSER_DEBUG) std::cerr << "Zrobiłem deklaracje, teraz robię zmienną" << std::endl;  ComplexCommand* complexCommand = new ComplexCommand();  complexCommand->pushCommandBack($1);  complexCommand->pushCommandBack(variableDirector->declareVariable(std::string($3)));   $$ = complexCommand;   } // { ComplexCommand* complexCommand = new ComplexCommand(); complexCommand->pushCommandBack($1); complexCommand->pushCommandBack(variableDirector->declareVariable($3)); $$ = complexCommand; }
                | declarations TOKEN_COMMA pidentifier TOKEN_LEFT_SQUARE_BRACKET num TOKEN_COLON num TOKEN_RIGHT_SQUARE_BRACKET {if(PARSER_DEBUG) std::cerr << "Zrobiłem deklaracje, teraz robię tablice" << std::endl;  ComplexCommand* complexCommand = new ComplexCommand(); complexCommand->pushCommandBack($1); complexCommand->pushCommandBack(variableDirector->declareArray(std::string($3), $5, $7)); $$ = complexCommand; }
                | pidentifier  {if(PARSER_DEBUG)  std::cerr << "Robię zmienną" << std::endl; ComplexCommand* complexCommand = new ComplexCommand(); complexCommand->pushCommandBack(variableDirector->declareVariable(std::string($1))); $$ = complexCommand; }
                | pidentifier TOKEN_LEFT_SQUARE_BRACKET num TOKEN_COLON num TOKEN_RIGHT_SQUARE_BRACKET {if(PARSER_DEBUG) std::cerr << "Robię tablice" << std::endl; $$ = 0; ComplexCommand* complexCommand = new ComplexCommand(); complexCommand->pushCommandBack(variableDirector->declareArray($1, $3, $5)); $$ = complexCommand; }
                | num { }

commands:         commands command { if(PARSER_DEBUG) std::cerr << "Tworzę ciąg komend" << std::endl; ComplexCommand* complexCommand = new ComplexCommand(); complexCommand->pushCommandBack($1); complexCommand->pushCommandBack($2); $$ = complexCommand; }
                | command { if(PARSER_DEBUG) std::cerr << "Tworzę pojedyńczą komende" << std::endl; $$ = $1; }

command:          identifier ASSIGN expression TOKEN_SEMICOLON {if(PARSER_DEBUG)  std::cerr << "Przpisuje wartość zmiennej" << std::endl; ComplexCommand* complexCommand = new ComplexCommand(); complexCommand->pushCommandBack($1); complexCommand->pushCommandBack($3); complexCommand->pushCommandBack(variableDirector->assign());  $$ = complexCommand; }

expression:       value
                | value TOKEN_PLUS value { ComplexCommand* complexCommand = new ComplexCommand(); complexCommand->pushCommandBack($1); complexCommand->pushCommandBack($3); complexCommand->pushCommandBack(new ADD_TWO_NUMBERS_FROM_STACK_AND_PUSH_RESULT_ON_STACK()); $$ = complexCommand; }
                | value TOKEN_MINUS value { ComplexCommand* complexCommand = new ComplexCommand(); complexCommand->pushCommandBack($1); complexCommand->pushCommandBack($3); complexCommand->pushCommandBack(new SUBTRACT_TWO_NUMBERS_FROM_STACK_AND_PUSH_RESULT_ON_STACK()); $$ = complexCommand; }

value:            num { ComplexCommand* complexCommand = new ComplexCommand(); complexCommand->pushCommandBack(new PUSH($1)); $$ = complexCommand; }
                | identifier { ComplexCommand* complexCommand = new ComplexCommand(); complexCommand->pushCommandBack($1); complexCommand->pushCommandBack(variableDirector->pushVariableOntoStackByAddressOfVariableFromStack());  $$ = complexCommand; } 

identifier:       pidentifier { ComplexCommand* complexCommand = new ComplexCommand(); complexCommand->pushCommandBack(variableDirector->pushAddressOfVariableOntoStack(std::string($1))); $$ = complexCommand;  }
                | pidentifier TOKEN_LEFT_SQUARE_BRACKET num TOKEN_RIGHT_SQUARE_BRACKET { ComplexCommand* complexCommand = new ComplexCommand(); complexCommand->pushCommandBack(variableDirector->pushAddressOfVariableFromArrayOntoStack(std::string($1), $3)); $$ = complexCommand;  }
                | pidentifier TOKEN_LEFT_SQUARE_BRACKET pidentifier TOKEN_RIGHT_SQUARE_BRACKET  { ComplexCommand* complexCommand = new ComplexCommand(); complexCommand->pushCommandBack(variableDirector->pushAddressOfVariableOntoStack(std::string($3))); complexCommand->pushCommandBack(variableDirector->getIndexFromStackAndPushAddressOfVariableFromArrayOntoStack($1)); $$ = complexCommand; }
                
%%

int yyerror(std::string error) {	
    std::cout << error << std::endl;
}

int main() {
    std::cerr << std::endl;
    variableDirector = new VariableDirector();
    yyparse();
    return 0;
}

/*
%token <number> EQ_TOKEN
%token <number> NEQ_TOKEN
%token <number> LE_TOKEN
%token <number> GE_TOKEN
%token <number> LEQ_TOKEN
%token <number> GEQ_TOKEN

condition:        value EQ_TOKEN value { ComplexCommand* complexCommand = new ComplexCommand(); complexCommand->pushCommandBack(new ADD_TWO_NUMBERS_FROM_STACK_AND_PUSH_RESULT_ON_STACK()); $$ = complexCommand; }
                | value NEQ_TOKEN value { ComplexCommand* complexCommand = new ComplexCommand(); complexCommand->pushCommandBack(new ADD_TWO_NUMBERS_FROM_STACK_AND_PUSH_RESULT_ON_STACK()); $$ = complexCommand; }
                | value LE_TOKEN value { ComplexCommand* complexCommand = new ComplexCommand(); complexCommand->pushCommandBack(new ADD_TWO_NUMBERS_FROM_STACK_AND_PUSH_RESULT_ON_STACK()); $$ = complexCommand; }
                | value GE_TOKEN value { ComplexCommand* complexCommand = new ComplexCommand(); complexCommand->pushCommandBack(new ADD_TWO_NUMBERS_FROM_STACK_AND_PUSH_RESULT_ON_STACK()); $$ = complexCommand; }
                | value LEQ_TOKEN value { ComplexCommand* complexCommand = new ComplexCommand(); complexCommand->pushCommandBack(new ADD_TWO_NUMBERS_FROM_STACK_AND_PUSH_RESULT_ON_STACK()); $$ = complexCommand; }
                | value GEQ_TOKEN value { ComplexCommand* complexCommand = new ComplexCommand(); complexCommand->pushCommandBack(new ADD_TWO_NUMBERS_FROM_STACK_AND_PUSH_RESULT_ON_STACK()); $$ = complexCommand; }

"EQ"     { if (SCANNER_DEBUG) std::cerr << "Rozpoznałem EQ" << std::endl; return EQ_TOKEN; }
"NEQ"   { if (SCANNER_DEBUG) std::cerr << "Rozpoznałem EQ" << std::endl; return NEQ_TOKEN; }
"LE"    { if (SCANNER_DEBUG) std::cerr << "Rozpoznałem EQ" << std::endl; return LE_TOKEN; }
"GE"    { if (SCANNER_DEBUG) std::cerr << "Rozpoznałem EQ" << std::endl; return GE_TOKEN; }
"LEQ"   { if (SCANNER_DEBUG) std::cerr << "Rozpoznałem EQ" << std::endl; return LEQ_TOKEN; }
"GEQ"   { if (SCANNER_DEBUG) std::cerr << "Rozpoznałem EQ" << std::endl; return GEQ_TOKEN; }

*/