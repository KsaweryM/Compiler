%{
    #include <iostream>
    #include "../inc/Command.h"
    #include "../inc/ComplexCommand.h"
    #include "../inc/VariableDirector.h"
    #include "../inc/Commands/commands.h"

    extern int yylex();
    extern int yyparse();
    extern void set_input_file(FILE* file);
    extern void yylex_destroy();
    int yyerror(std::string);

    VariableDirector* variableDirector;

    #define PARSER_DEBUG 0
    #define DISPLAY_STACK_END 0
%}

%union
{
    long long int number;
    char* text;
    Command* com;
}

%token <number> TOKEN_IF
%token <number> TOKEN_THEN
%token <number> TOKEN_ELSE
%token <number> TOKEN_ENDIF

%token <number> TOKEN_VAR
%token <number> TOKEN_BEGIN
%token <number> TOKEN_END
%token <number> ASSIGN
%left  <number> TOKEN_PLUS
%left  <number> TOKEN_MINUS
%left  <number> TOKEN_TIMES
%left  <number> TOKEN_DIV
%left  <number> TOKEN_MOD
%token <number> TOKEN_LEFT_SQUARE_BRACKET
%token <number> TOKEN_RIGHT_SQUARE_BRACKET
%token <number> TOKEN_SEMICOLON
%token <number> TOKEN_COLON
%token <number> TOKEN_COMMA
%token <number> TOKEN_ERROR

%token <number> EQ_TOKEN
%token <number> NEQ_TOKEN
%token <number> LE_TOKEN
%token <number> GE_TOKEN
%token <number> LEQ_TOKEN
%token <number> GEQ_TOKEN

%token <number> TOKEN_WRITE
%token <number> TOKEN_READ

%token <number> TOKEN_WHILE
%token <number> TOKEN_DO
%token <number> TOKEN_END_WHILE

%token <number> TOKEN_REPEAT
%token <number> TOKEN_UNTIL
%token <number> TOKEN_FOR
%token <number> TOKEN_FROM
%token <number> TOKEN_TO
%token <number> TOKEN_DOWNTO
%token <number> TOKEN_ENDFOR

%token <text> pidentifier
%token <number> num

%type <com> iterator
%type <com> iterator2

%type <com> input
%type <com> program
%type <com> declarations
%type <com> commands
%type <com> command
%type <com> identifier
%type <com> expression
%type <com> condition
%type <com> value
%%
input: program

program:          TOKEN_VAR declarations TOKEN_BEGIN commands TOKEN_END  {if(PARSER_DEBUG) std::cerr << "Tworzę program" << std::endl; ComplexCommand* complexCommand = new ComplexCommand(); complexCommand->pushCommandBack(new RESET_STACK()); complexCommand->pushCommandBack($2); complexCommand->pushCommandBack($4); if (DISPLAY_STACK_END) complexCommand->pushCommandBack(new DISPLAY_STACK_N(10));  complexCommand->pushCommandBack(new HALT());   complexCommand->execute(); delete complexCommand;  }
                | TOKEN_BEGIN commands TOKEN_END { $$ = $2; }

declarations:     declarations TOKEN_COMMA pidentifier { if(PARSER_DEBUG) std::cerr << "Zrobiłem deklaracje, teraz robię zmienną" << std::endl;  ComplexCommand* complexCommand = new ComplexCommand();  complexCommand->pushCommandBack($1);  complexCommand->pushCommandBack(variableDirector->declareVariable(std::string($3))); delete $3;  $$ = complexCommand;   } // { ComplexCommand* complexCommand = new ComplexCommand(); complexCommand->pushCommandBack($1); complexCommand->pushCommandBack(variableDirector->declareVariable($3)); $$ = complexCommand; }
                | declarations TOKEN_COMMA pidentifier TOKEN_LEFT_SQUARE_BRACKET num TOKEN_COLON num TOKEN_RIGHT_SQUARE_BRACKET {if(PARSER_DEBUG) std::cerr << "Zrobiłem deklaracje, teraz robię tablice" << std::endl;  ComplexCommand* complexCommand = new ComplexCommand(); complexCommand->pushCommandBack($1); complexCommand->pushCommandBack(variableDirector->declareArray(std::string($3), $5, $7)); delete $3; $$ = complexCommand; }
                | pidentifier  {if(PARSER_DEBUG)  std::cerr << "Robię zmienną" << std::endl; ComplexCommand* complexCommand = new ComplexCommand(); complexCommand->pushCommandBack(variableDirector->declareVariable(std::string($1))); delete $1; $$ = complexCommand; }
                | pidentifier TOKEN_LEFT_SQUARE_BRACKET num TOKEN_COLON num TOKEN_RIGHT_SQUARE_BRACKET {if(PARSER_DEBUG) std::cerr << "Robię tablice" << std::endl; $$ = 0; ComplexCommand* complexCommand = new ComplexCommand(); complexCommand->pushCommandBack(variableDirector->declareArray(std::string($1), $3, $5)); delete $1; $$ = complexCommand; }

commands:         commands command { if(PARSER_DEBUG) std::cerr << "Tworzę ciąg komend" << std::endl; ComplexCommand* complexCommand = new ComplexCommand(); complexCommand->pushCommandBack($1); complexCommand->pushCommandBack($2); $$ = complexCommand; }
                | command { if(PARSER_DEBUG) std::cerr << "Tworzę pojedyńczą komende" << std::endl; $$ = $1; }

command:          identifier ASSIGN expression TOKEN_SEMICOLON {if(PARSER_DEBUG)  std::cerr << "Przpisuje wartość zmiennej" << std::endl; ComplexCommand* complexCommand = new ComplexCommand(); if($1->isIterator()) {  throw std::invalid_argument("Cannot assign value to iterator!"); } complexCommand->pushCommandBack($1); complexCommand->pushCommandBack($3); complexCommand->pushCommandBack(variableDirector->assign());  $$ = complexCommand; }
                | TOKEN_WRITE value TOKEN_SEMICOLON { ComplexCommand* complexCommand = new ComplexCommand(); complexCommand->pushCommandBack($2); complexCommand->pushCommandBack(new WRITE()); $$ = complexCommand; }
                | TOKEN_READ identifier TOKEN_SEMICOLON { ComplexCommand* complexCommand = new ComplexCommand(); complexCommand->pushCommandBack($2); complexCommand->pushCommandBack(new READ()); $$ = complexCommand;}
                | TOKEN_IF condition TOKEN_THEN commands TOKEN_ELSE commands TOKEN_ENDIF { $$ = new IF($2, $4, $6); }
                | TOKEN_IF condition TOKEN_THEN commands TOKEN_ENDIF { $$ = new IF_THEN($2, $4); }
                | TOKEN_WHILE condition TOKEN_DO commands TOKEN_END_WHILE { $$ = new WHILE($2, $4); }
                | TOKEN_REPEAT commands TOKEN_UNTIL condition TOKEN_SEMICOLON { $$ = new REPEAT($2, $4); }
                | TOKEN_FOR iterator TOKEN_DO commands TOKEN_ENDFOR { 
                    ComplexCommand* complexCommand = new ComplexCommand(); 
                    complexCommand->pushCommandBack($2);
                    
                    ComplexCommand* condition = new ComplexCommand(); 
                    condition->pushCommandBack(variableDirector->pushAddressOfVariableOntoStack($2->getIteratorName()));
                    condition->pushCommandBack(variableDirector->pushVariableOntoStackByAddressOfVariableFromStack());
                    condition->pushCommandBack(variableDirector->pushAddressOfAnonymousVariableOntoStack($2->getAnonymousIndex()));
                    condition->pushCommandBack(variableDirector->pushVariableOntoStackByAddressOfVariableFromStack());
                    condition->pushCommandBack(new LEQ_CONDITION());
                    ComplexCommand* loopBody = new ComplexCommand();
                    loopBody->pushCommandBack($4);
                    loopBody->pushCommandBack(variableDirector->incrementIterator($2->getIteratorName()));
                    complexCommand->pushCommandBack(new WHILE(condition, loopBody));
                    $$ = complexCommand; }
                | TOKEN_FOR iterator2 TOKEN_DO commands TOKEN_ENDFOR { 
                    ComplexCommand* complexCommand = new ComplexCommand(); 
                    complexCommand->pushCommandBack($2);
                    
                    ComplexCommand* condition = new ComplexCommand(); 
                    condition->pushCommandBack(variableDirector->pushAddressOfVariableOntoStack($2->getIteratorName()));
                    condition->pushCommandBack(variableDirector->pushVariableOntoStackByAddressOfVariableFromStack());
                    condition->pushCommandBack(variableDirector->pushAddressOfAnonymousVariableOntoStack($2->getAnonymousIndex()));
                    condition->pushCommandBack(variableDirector->pushVariableOntoStackByAddressOfVariableFromStack());
                    condition->pushCommandBack(new GEQ_CONDITION());
                    ComplexCommand* loopBody = new ComplexCommand();
                    loopBody->pushCommandBack($4);
                    loopBody->pushCommandBack(variableDirector->decrementIterator($2->getIteratorName()));
                    complexCommand->pushCommandBack(new WHILE(condition, loopBody));
                    $$ = complexCommand; }


iterator:   pidentifier TOKEN_FROM value TOKEN_TO value { 
                                    ComplexCommand* complexCommand = new ComplexCommand(); 
                                    complexCommand->pushCommandBack(variableDirector->declareIterator(std::string($1)));
                                    delete $1;
                                    complexCommand->pushCommandBack(variableDirector->pushAddressOfVariableOntoStack(std::string($1)));
                                    complexCommand->pushCommandBack($3); 
                                    complexCommand->pushCommandBack(variableDirector->assign());

                                    int anonymousIndex = -1;
                                    complexCommand->pushCommandBack(variableDirector->declareAnonymousVariable(&anonymousIndex));
                                    complexCommand->pushCommandBack(variableDirector->pushAddressOfAnonymousVariableOntoStack(anonymousIndex));
                                    complexCommand->pushCommandBack($5);
                                    complexCommand->pushCommandBack(variableDirector->assign());

                                    complexCommand->setIteratorName(std::string($1));
                                    complexCommand->setAnonymousIndex(anonymousIndex);

                                    $$ = complexCommand; }

iterator2:   pidentifier TOKEN_FROM value TOKEN_DOWNTO value { 
                                    ComplexCommand* complexCommand = new ComplexCommand(); 
                                    complexCommand->pushCommandBack(variableDirector->declareIterator(std::string($1)));
                                    delete $1;
                                    complexCommand->pushCommandBack(variableDirector->pushAddressOfVariableOntoStack(std::string($1)));
                                    complexCommand->pushCommandBack($3); 
                                    complexCommand->pushCommandBack(variableDirector->assign());

                                    int anonymousIndex = -1;
                                    complexCommand->pushCommandBack(variableDirector->declareAnonymousVariable(&anonymousIndex));
                                    complexCommand->pushCommandBack(variableDirector->pushAddressOfAnonymousVariableOntoStack(anonymousIndex));
                                    complexCommand->pushCommandBack($5);
                                    complexCommand->pushCommandBack(variableDirector->assign());

                                    complexCommand->setIteratorName(std::string($1));
                                    complexCommand->setAnonymousIndex(anonymousIndex);

                                    $$ = complexCommand; }                                    

expression:       value
                | value TOKEN_PLUS value { ComplexCommand* complexCommand = new ComplexCommand(); complexCommand->pushCommandBack($1); complexCommand->pushCommandBack($3); complexCommand->pushCommandBack(new ADD_TWO_NUMBERS_FROM_STACK_AND_PUSH_RESULT_ON_STACK()); $$ = complexCommand; }
                | value TOKEN_MINUS value { ComplexCommand* complexCommand = new ComplexCommand(); complexCommand->pushCommandBack($1); complexCommand->pushCommandBack($3); complexCommand->pushCommandBack(new SUBTRACT_TWO_NUMBERS_FROM_STACK_AND_PUSH_RESULT_ON_STACK()); $$ = complexCommand; }
                | value TOKEN_TIMES value { ComplexCommand* complexCommand = new ComplexCommand(); complexCommand->pushCommandBack($1); complexCommand->pushCommandBack($3); complexCommand->pushCommandBack(new TIMES_TWO_NUMBERS_FROM_STACK_AND_PUSH_RESULT_ON_STACK()); $$ = complexCommand; }
                | value TOKEN_DIV value { ComplexCommand* complexCommand = new ComplexCommand(); complexCommand->pushCommandBack($1); complexCommand->pushCommandBack($3); complexCommand->pushCommandBack(new DIV_TWO_NUMBERS_FROM_STACK_AND_PUSH_RESULT_ON_STACK()); $$ = complexCommand; }
                | value TOKEN_MOD value { ComplexCommand* complexCommand = new ComplexCommand(); complexCommand->pushCommandBack($1); complexCommand->pushCommandBack($3); complexCommand->pushCommandBack(new MOD_TWO_NUMBERS_FROM_STACK_AND_PUSH_RESULT_ON_STACK()); $$ = complexCommand; }

condition:        value EQ_TOKEN value { ComplexCommand* complexCommand = new ComplexCommand(); complexCommand->pushCommandBack($1); complexCommand->pushCommandBack($3); complexCommand->pushCommandBack(new EQ_CONDITION()); $$ = complexCommand; }
                | value NEQ_TOKEN value { ComplexCommand* complexCommand = new ComplexCommand(); complexCommand->pushCommandBack($1); complexCommand->pushCommandBack($3); complexCommand->pushCommandBack(new NEQ_CONDITION()); $$ = complexCommand; }
                | value LE_TOKEN value { ComplexCommand* complexCommand = new ComplexCommand(); complexCommand->pushCommandBack($1); complexCommand->pushCommandBack($3); complexCommand->pushCommandBack(new LE_CONDITION()); $$ = complexCommand; }
                | value GE_TOKEN value { ComplexCommand* complexCommand = new ComplexCommand(); complexCommand->pushCommandBack($1); complexCommand->pushCommandBack($3); complexCommand->pushCommandBack(new GE_CONDITION()); $$ = complexCommand; }
                | value LEQ_TOKEN value { ComplexCommand* complexCommand = new ComplexCommand(); complexCommand->pushCommandBack($1); complexCommand->pushCommandBack($3); complexCommand->pushCommandBack(new LEQ_CONDITION()); $$ = complexCommand; }
                | value GEQ_TOKEN value { ComplexCommand* complexCommand = new ComplexCommand(); complexCommand->pushCommandBack($1); complexCommand->pushCommandBack($3); complexCommand->pushCommandBack(new GEQ_CONDITION()); $$ = complexCommand; }

value:            num { ComplexCommand* complexCommand = new ComplexCommand(); complexCommand->pushCommandBack(new PUSH($1)); $$ = complexCommand; }
                | identifier { ComplexCommand* complexCommand = new ComplexCommand(); complexCommand->pushCommandBack($1); complexCommand->pushCommandBack(variableDirector->pushVariableOntoStackByAddressOfVariableFromStack());  $$ = complexCommand; } 

identifier:       pidentifier { ComplexCommand* complexCommand = new ComplexCommand(); complexCommand->pushCommandBack(variableDirector->pushAddressOfVariableOntoStack(std::string($1))); if(variableDirector->isIterator(std::string($1))) complexCommand->setAsIterator(); delete $1; $$ = complexCommand;  }
                | pidentifier TOKEN_LEFT_SQUARE_BRACKET num TOKEN_RIGHT_SQUARE_BRACKET { ComplexCommand* complexCommand = new ComplexCommand(); complexCommand->pushCommandBack(variableDirector->pushAddressOfVariableFromArrayOntoStack(std::string($1), $3)); delete $1; $$ = complexCommand;  }
                | pidentifier TOKEN_LEFT_SQUARE_BRACKET pidentifier TOKEN_RIGHT_SQUARE_BRACKET  { ComplexCommand* complexCommand = new ComplexCommand(); complexCommand->pushCommandBack(variableDirector->pushAddressOfVariableOntoStack(std::string($3))); complexCommand->pushCommandBack(variableDirector->getIndexFromStackAndPushAddressOfVariableFromArrayOntoStack(std::string($1))); delete $1; delete $3; $$ = complexCommand; }
                
%%

int yyerror(std::string error) {	
    std::cout << error << std::endl;
}

int main(int argc, char** argv) {
    variableDirector = new VariableDirector();
    
    if (argc != 3) {
        std::string text = "Nie podano nazw wszystkich plików!";
        throw std::invalid_argument(text);
    }

    //FILE* file = fopen("test/input.txt" , "r");
    FILE* file = fopen(argv[1] , "r");
    if (file == NULL) {
        std::string text = "Plik wejściowy o podanej nazwie nie istnieje!";
        throw std::invalid_argument(text);
    }

    freopen(argv[2], "a+", stdout);


    set_input_file(file);
    yyparse();
    yylex_destroy();
    fclose(file);
    
    delete variableDirector;
    return 0;
}