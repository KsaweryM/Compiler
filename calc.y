%{
    #include <iostream>
    
    extern int yylex();
    extern int yyparse();
    int yyerror(std::string);

int createNumber(int n) {
    if (n <= 1) {
        std::cout << "RESET a" << std::endl;
        std::cout << "RESET h" << std::endl;
        std::cout << "INC h" << std::endl;

        if (n == 1) {
            std::cout << "ADD h" << std::endl;    
        }
    }
    else {
        createNumber(n / 2);
        std::cout << "SHIFT h" << std::endl;

        if (n % 2) {
            std::cout << "ADD h" << std::endl;
        }
    }
}

void push(int n) {
    // n jest w rejestrze a
    createNumber(n);
    // inkrementuj stos
    std::cout << "INC c" << std::endl; 
    // zapisz n na stosie 
    std::cout << "STORE c" << std::endl;
}

void pop() {
    std::cout << "LOAD c" << std::endl;
    std::cout << "DEC c" << std::endl;
}


void add() {
    pop();
    std::cout << "RESET b" << std::endl;
    std::cout << "SWAP b" << std::endl;
    pop();
    std::cout << "ADD b" << std::endl;
    std::cout << "INC c" << std::endl;
    std::cout << "STORE c" << std::endl;
}

void sub() {
    pop();
    std::cout << "RESET b" << std::endl;
    std::cout << "SWAP b" << std::endl;
    pop();
    std::cout << "SUB b" << std::endl;
    std::cout << "INC c" << std::endl;
    std::cout << "STORE c" << std::endl;
}

void resetStack() {
    std::cout << "RESET c" << std::endl;
}

void end() {
    std::cout << "PUT" << std::endl;
    std::cout << "HALT" << std::endl;
}    
void neg() {
    std::cout << "LOAD c" << std::endl;
    std::cout << "RESET b" << std::endl;
    std::cout << "SWAP b" << std::endl;
    std::cout << "SUB b" << std::endl;
    std::cout << "STORE c" << std::endl;
}

void multi() {
    pop();
    std::cout << "SWAP b" << std::endl;    
    pop();
    std::cout << "SWAP d" << std::endl;
    std::cout << "RESET a" << std::endl;
    std::cout << "SWAP d" << std::endl;
    std::cout << "JZERO 5" << std::endl;
    std::cout << "SWAP d" << std::endl;
    std::cout << "ADD b" << std::endl;
    std::cout << "DEC d" << std::endl;
    std::cout << "JUMP -5" << std::endl;
    std::cout << "SWAP d" << std::endl;
    std::cout << "INC c" << std::endl;
    std::cout << "STORE c" << std::endl;
}

%}

%token NUMBER

%left PLUS
%left MINUS
%left MULTI
%left DIV
%token LNAW
%token PNAW
%token ERROR

%token END
%token FINISH
%precedence NEG
%%
input: %empty
    | input line 
;
line: exp END {   end();     }
    | END {    }
;

exp: NUMBER                { push($1);       }
    | exp PLUS exp         { add();          }
    | exp MINUS exp        { sub();      }
    | exp MULTI exp        { multi(); }
    | LNAW exp PNAW 
    | MINUS exp %prec NEG {  neg(); std::cerr << "NEG" << std::endl; }
%%

int yyerror(std::string error) {	
    std::cout << error << std::endl;
}

int main() {
    resetStack();
    yyparse();
    return 0;
}