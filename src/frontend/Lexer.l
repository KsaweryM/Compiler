%{
// Lexical analyzer: splits the source text into tokens for the parser.

#include <stdexcept>
#include <string>

#include "Parser.hpp"
#include "common/Diagnostics.h"
#include "frontend/Frontend.h"

#define YY_DECL yy::parser::symbol_type yylex()
#define YY_USER_ACTION currentLocation.columns(yyleng);

namespace {

yy::location currentLocation;

long long parseNumber(const char* text) {
    try {
        return std::stoll(text);
    }
    catch (const std::out_of_range&) {
        throw CompileError(currentLocation.begin.line, std::string("number ") + text + " is out of range");
    }
}

}
%}

%option noyywrap nounput noinput nodefault batch

%x COMMENT

%%

%{
    currentLocation.step();
%}

"("                 BEGIN(COMMENT);
<COMMENT>")"        BEGIN(INITIAL); currentLocation.step();
<COMMENT>\n         currentLocation.lines(1);
<COMMENT>[^)\n]+    /* comment text */
<COMMENT><<EOF>>    throw CompileError(currentLocation.begin.line, "unterminated comment");

\n                  currentLocation.lines(1); currentLocation.step();
[ \t\r]+            currentLocation.step();

"VAR"               return yy::parser::make_VAR(currentLocation);
"BEGIN"             return yy::parser::make_BEGIN(currentLocation);
"END"               return yy::parser::make_END(currentLocation);

"ASSIGN"            return yy::parser::make_ASSIGN(currentLocation);
";"                 return yy::parser::make_SEMICOLON(currentLocation);
":"                 return yy::parser::make_COLON(currentLocation);
","                 return yy::parser::make_COMMA(currentLocation);
"["                 return yy::parser::make_LEFT_BRACKET(currentLocation);
"]"                 return yy::parser::make_RIGHT_BRACKET(currentLocation);

"PLUS"              return yy::parser::make_PLUS(currentLocation);
"MINUS"             return yy::parser::make_MINUS(currentLocation);
"TIMES"             return yy::parser::make_TIMES(currentLocation);
"DIV"               return yy::parser::make_DIV(currentLocation);
"MOD"               return yy::parser::make_MOD(currentLocation);

"EQ"                return yy::parser::make_EQ(currentLocation);
"NEQ"               return yy::parser::make_NEQ(currentLocation);
"LE"                return yy::parser::make_LE(currentLocation);
"GE"                return yy::parser::make_GE(currentLocation);
"LEQ"               return yy::parser::make_LEQ(currentLocation);
"GEQ"               return yy::parser::make_GEQ(currentLocation);

"IF"                return yy::parser::make_IF(currentLocation);
"THEN"              return yy::parser::make_THEN(currentLocation);
"ELSE"              return yy::parser::make_ELSE(currentLocation);
"ENDIF"             return yy::parser::make_ENDIF(currentLocation);
"WHILE"             return yy::parser::make_WHILE(currentLocation);
"DO"                return yy::parser::make_DO(currentLocation);
"ENDWHILE"          return yy::parser::make_ENDWHILE(currentLocation);
"REPEAT"            return yy::parser::make_REPEAT(currentLocation);
"UNTIL"             return yy::parser::make_UNTIL(currentLocation);
"FOR"               return yy::parser::make_FOR(currentLocation);
"FROM"              return yy::parser::make_FROM(currentLocation);
"TO"                return yy::parser::make_TO(currentLocation);
"DOWNTO"            return yy::parser::make_DOWNTO(currentLocation);
"ENDFOR"            return yy::parser::make_ENDFOR(currentLocation);
"READ"              return yy::parser::make_READ(currentLocation);
"WRITE"             return yy::parser::make_WRITE(currentLocation);

[_A-Za-z]+          return yy::parser::make_PIDENTIFIER(yytext, currentLocation);
-?[0-9]+            return yy::parser::make_NUM(parseNumber(yytext), currentLocation);

.                   throw CompileError(currentLocation.begin.line, std::string("unexpected character '") + yytext + "'");

<<EOF>>             return yy::parser::make_END_OF_FILE(currentLocation);

%%

ast::Program parseProgram(std::FILE* input) {
    yyrestart(input);
    BEGIN(INITIAL);
    currentLocation.initialize();

    ast::Program program;
    yy::parser parser(program);

    try {
        parser.parse();
    }
    catch (...) {
        yylex_destroy();
        throw;
    }

    yylex_destroy();
    return program;
}
