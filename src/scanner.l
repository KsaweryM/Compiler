%{
#include <iostream>
#include <string>
#include "../inc/Command.h"
#include "parser_y.h"

#define SCANNER_DEBUG 0
int yylex();
%}

%%
[ \t]+  { if (SCANNER_DEBUG) std::cerr << "Rozpoznałem VAR" << std::endl;   }
^#.*\n { if (SCANNER_DEBUG) std::cerr << "Rozpoznałem komentarz" << std::endl;  }
"VAR"     { if (SCANNER_DEBUG) std::cerr << "Rozpoznałem VAR" << std::endl;  return TOKEN_VAR; }
"BEGIN"   {  if (SCANNER_DEBUG) std::cerr << "Rozpoznałem BEGIN" << std::endl; return TOKEN_BEGIN; }
"END"     { if (SCANNER_DEBUG) std::cerr << "Rozpoznałem END" << std::endl;   return TOKEN_END; }
"ASSIGN"  {  if (SCANNER_DEBUG) std::cerr << "Rozpoznałem ASSIGN" << std::endl;  return ASSIGN; }
"+"      {  if (SCANNER_DEBUG) std::cerr << "Rozpoznałem +" << std::endl;  return TOKEN_PLUS;                               }
"-"      {  if (SCANNER_DEBUG) std::cerr << "Rozpoznałem -" << std::endl;  return TOKEN_MINUS;                               }
"["      { if (SCANNER_DEBUG) std::cerr << "Rozpoznałem [" << std::endl;   return TOKEN_LEFT_SQUARE_BRACKET;                               }
"]"      {  if (SCANNER_DEBUG) std::cerr << "Rozpoznałem ]" << std::endl;  return TOKEN_RIGHT_SQUARE_BRACKET;                               }
";"      {  if (SCANNER_DEBUG) std::cerr << "Rozpoznałem ;" << std::endl;  return TOKEN_SEMICOLON; }
":"      {   if (SCANNER_DEBUG) std::cerr << "Rozpoznałem :" << std::endl;  return TOKEN_COLON; }
","      {  if (SCANNER_DEBUG) std::cerr << "Rozpoznałem ," << std::endl;  return TOKEN_COMMA; }
[A-Za-z][A-Za-z0-9]* {  if (SCANNER_DEBUG) std::cerr << "Rozpoznałem zmienną" << std::endl;  yylval.text = strdup(yytext); return pidentifier; }
-?[0-9]+  { if (SCANNER_DEBUG) std::cerr << "Rozpoznałem liczbę" << std::endl;  yylval.number = atoi(yytext);  return num; }
.       {  if (SCANNER_DEBUG) std::cerr << "Nie rozpoznałem tego symbolu " << " - " << int(*yytext) << std::endl;  return TOKEN_ERROR;                              }


%%

int yywrap() {
    return 1;
}