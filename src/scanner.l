%{
#include <iostream>
#include <string>
#include "../inc/Command.h"
#include "parser_y.h"

int yylex();
%}

%%
[ \t] ;
^#.*\n ;
"VAR"     { return TOKEN_VAR; }
"BEGIN"   { return TOKEN_BEGIN; }
"END"     { return TOKEN_END; }
"ASSIGN"  { return ASSIGN; }
"+"      { return TOKEN_PLUS;                               }
"-"      { return TOKEN_MINUS;                              }
"["      { return TOKEN_LEFT_SQUARE_BRACKET;                               }
"]"      { return TOKEN_RIGHT_SQUARE_BRACKET;                               }
";"      { return TOKEN_SEMICOLON; }
":"      { return TOKEN_COLON; }
","      { return TOKEN_COMMA; }
[A-Za-z][A-Za-z0-9]* { yylval.text = strdup(yytext); return pidentifier; }
[0-9]+  {yylval.number = atoi(yytext);  return num; }
.       { return TOKEN_ERROR;                              }


%%

int yywrap() {
    return 1;
}