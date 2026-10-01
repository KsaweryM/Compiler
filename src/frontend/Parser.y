// Grammar of the source language. The actions only build the abstract syntax tree;
// all checks happen later, in the semantic analyzer.

%require "3.2"
%language "c++"

%define api.value.type variant
%define api.token.constructor
%define api.token.prefix {TOKEN_}
%define api.location.file none
%define parse.assert
%define parse.error verbose
%locations

%parse-param {ast::Program& program}

%code requires {
    #include <string>
    #include <vector>
    #include "frontend/Ast.h"
}

%code {
    #include "common/Diagnostics.h"

    yy::parser::symbol_type yylex();
}

%token END_OF_FILE 0 "end of file"
%token VAR "VAR" BEGIN "BEGIN" END "END"
%token ASSIGN "ASSIGN" SEMICOLON ";" COLON ":" COMMA "," LEFT_BRACKET "[" RIGHT_BRACKET "]"
%token PLUS "PLUS" MINUS "MINUS" TIMES "TIMES" DIV "DIV" MOD "MOD"
%token EQ "EQ" NEQ "NEQ" LE "LE" GE "GE" LEQ "LEQ" GEQ "GEQ"
%token IF "IF" THEN "THEN" ELSE "ELSE" ENDIF "ENDIF"
%token WHILE "WHILE" DO "DO" ENDWHILE "ENDWHILE"
%token REPEAT "REPEAT" UNTIL "UNTIL"
%token FOR "FOR" FROM "FROM" TO "TO" DOWNTO "DOWNTO" ENDFOR "ENDFOR"
%token READ "READ" WRITE "WRITE"
%token <std::string> PIDENTIFIER "identifier"
%token <long long> NUM "number"

%type <std::vector<ast::Declaration>> declarations
%type <ast::CommandList> commands
%type <ast::Command> command
%type <ast::Expression> expression
%type <ast::Condition> condition
%type <ast::Value> value
%type <ast::Identifier> identifier

%%

program
    : VAR declarations BEGIN commands END
        { program.declarations = std::move($2); program.commands = std::move($4); }
    | BEGIN commands END
        { program.commands = std::move($2); }
    ;

declarations
    : declarations COMMA PIDENTIFIER
        { $$ = std::move($1); $$.push_back(ast::Declaration::scalar($3, @3.begin.line)); }
    | declarations COMMA PIDENTIFIER LEFT_BRACKET NUM COLON NUM RIGHT_BRACKET
        { $$ = std::move($1); $$.push_back(ast::Declaration::array($3, $5, $7, @3.begin.line)); }
    | PIDENTIFIER
        { $$.push_back(ast::Declaration::scalar($1, @1.begin.line)); }
    | PIDENTIFIER LEFT_BRACKET NUM COLON NUM RIGHT_BRACKET
        { $$.push_back(ast::Declaration::array($1, $3, $5, @1.begin.line)); }
    ;

commands
    : commands command      { $$ = std::move($1); $$.push_back(std::move($2)); }
    | command               { $$.push_back(std::move($1)); }
    ;

command
    : identifier ASSIGN expression SEMICOLON
        { $$ = ast::Command{@1.begin.line, ast::Assign{std::move($1), std::move($3)}}; }
    | IF condition THEN commands ELSE commands ENDIF
        { $$ = ast::Command{@1.begin.line, ast::If{std::move($2), std::move($4), std::move($6)}}; }
    | IF condition THEN commands ENDIF
        { $$ = ast::Command{@1.begin.line, ast::If{std::move($2), std::move($4), {}}}; }
    | WHILE condition DO commands ENDWHILE
        { $$ = ast::Command{@1.begin.line, ast::While{std::move($2), std::move($4)}}; }
    | REPEAT commands UNTIL condition SEMICOLON
        { $$ = ast::Command{@1.begin.line, ast::Repeat{std::move($2), std::move($4)}}; }
    | FOR PIDENTIFIER FROM value TO value DO commands ENDFOR
        { $$ = ast::Command{@1.begin.line, ast::For{$2, std::move($4), std::move($6), false, std::move($8)}}; }
    | FOR PIDENTIFIER FROM value DOWNTO value DO commands ENDFOR
        { $$ = ast::Command{@1.begin.line, ast::For{$2, std::move($4), std::move($6), true, std::move($8)}}; }
    | READ identifier SEMICOLON
        { $$ = ast::Command{@1.begin.line, ast::Read{std::move($2)}}; }
    | WRITE value SEMICOLON
        { $$ = ast::Command{@1.begin.line, ast::Write{std::move($2)}}; }
    ;

expression
    : value                 { $$ = ast::Expression{std::move($1), std::nullopt, {}}; }
    | value PLUS value      { $$ = ast::Expression{std::move($1), ast::Operator::PLUS, std::move($3)}; }
    | value MINUS value     { $$ = ast::Expression{std::move($1), ast::Operator::MINUS, std::move($3)}; }
    | value TIMES value     { $$ = ast::Expression{std::move($1), ast::Operator::TIMES, std::move($3)}; }
    | value DIV value       { $$ = ast::Expression{std::move($1), ast::Operator::DIV, std::move($3)}; }
    | value MOD value       { $$ = ast::Expression{std::move($1), ast::Operator::MOD, std::move($3)}; }
    ;

condition
    : value EQ value        { $$ = ast::Condition{std::move($1), ast::Relation::EQ, std::move($3)}; }
    | value NEQ value       { $$ = ast::Condition{std::move($1), ast::Relation::NEQ, std::move($3)}; }
    | value LE value        { $$ = ast::Condition{std::move($1), ast::Relation::LE, std::move($3)}; }
    | value GE value        { $$ = ast::Condition{std::move($1), ast::Relation::GE, std::move($3)}; }
    | value LEQ value       { $$ = ast::Condition{std::move($1), ast::Relation::LEQ, std::move($3)}; }
    | value GEQ value       { $$ = ast::Condition{std::move($1), ast::Relation::GEQ, std::move($3)}; }
    ;

value
    : NUM                   { $$ = ast::Value::number($1); }
    | identifier            { $$ = ast::Value::variable(std::move($1)); }
    ;

identifier
    : PIDENTIFIER
        { $$ = ast::Identifier::scalar($1, @1.begin.line); }
    | PIDENTIFIER LEFT_BRACKET NUM RIGHT_BRACKET
        { $$ = ast::Identifier::element($1, $3, @1.begin.line); }
    | PIDENTIFIER LEFT_BRACKET PIDENTIFIER RIGHT_BRACKET
        { $$ = ast::Identifier::element($1, $3, @1.begin.line); }
    ;

%%

void yy::parser::error(const location_type& location, const std::string& message) {
    throw CompileError(location.begin.line, message);
}
