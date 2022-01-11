%{
#include <iostream>
#include <string>
#include "../inc/Command.h"
#include "parser_y.h"

#define SCANNER_DEBUG 0
int yylex();
%}

%%
[ \t]+  { if (SCANNER_DEBUG) std::cerr << "Rozpoznałem biały znak" << std::endl;   }
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
"EQ"     { if (SCANNER_DEBUG) std::cerr << "Rozpoznałem EQ" << std::endl; return EQ_TOKEN; }
"NEQ"   { if (SCANNER_DEBUG) std::cerr << "Rozpoznałem EQ" << std::endl; return NEQ_TOKEN; }
"LE"    { if (SCANNER_DEBUG) std::cerr << "Rozpoznałem EQ" << std::endl; return LE_TOKEN; }
"GE"    { if (SCANNER_DEBUG) std::cerr << "Rozpoznałem EQ" << std::endl; return GE_TOKEN; }
"LEQ"   { if (SCANNER_DEBUG) std::cerr << "Rozpoznałem EQ" << std::endl; return LEQ_TOKEN; }
"GEQ"   { if (SCANNER_DEBUG) std::cerr << "Rozpoznałem EQ" << std::endl; return GEQ_TOKEN; }
"READ"  { if (SCANNER_DEBUG) std::cerr << "Rozpoznałem READ" << std::endl; return TOKEN_READ; }
"WRITE" { if (SCANNER_DEBUG) std::cerr << "Rozpoznałem WRITE" << std::endl; return TOKEN_WRITE; }
"IF"    { if (SCANNER_DEBUG) std::cerr << "Rozpoznałem IF" << std::endl; return TOKEN_IF; }
"THEN"  { if (SCANNER_DEBUG) std::cerr << "Rozpoznałem THEN" << std::endl; return TOKEN_THEN; }
"ELSE" { if (SCANNER_DEBUG) std::cerr << "Rozpoznałem ELSE" << std::endl; return TOKEN_ELSE; }
"ENDIF" { if (SCANNER_DEBUG) std::cerr << "Rozpoznałem ENDIF" << std::endl; return TOKEN_ENDIF; }
[A-Za-z][A-Za-z0-9]* {  if (SCANNER_DEBUG) std::cerr << "Rozpoznałem zmienną" << std::endl;  yylval.text = strdup(yytext); return pidentifier; }
-?[0-9]+  { if (SCANNER_DEBUG) std::cerr << "Rozpoznałem liczbę" << std::endl;  yylval.number = atoi(yytext);  return num; }
.       {  if (SCANNER_DEBUG) std::cerr << "Nie rozpoznałem tego symbolu " << " - " << int(*yytext) << std::endl;  return TOKEN_ERROR;                              }


%%

int yywrap() {
    return 1;
}