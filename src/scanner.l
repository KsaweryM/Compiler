%{
#include <iostream>
#include <string>
#include "../inc/Command.h"
#include "parser_y.h"

#define SCANNER_DEBUG 0
int yylex();
%}

%x COMMENT

%%

"("                                 {   if(SCANNER_DEBUG) std::cerr << "Rozpoznałem początek komentarza" << std::endl; BEGIN(COMMENT);   }
<COMMENT>")"                     {  if(SCANNER_DEBUG) std::cerr << "Rozpoznałem koniec komentarza" << std::endl; BEGIN(INITIAL);     }
<COMMENT>\n                     { if(SCANNER_DEBUG) std::cerr << "Rozpoznałem nową linię w komentarzu" << std::endl;              }
<COMMENT>[^"("")"]|[^")"]|[^\n]  { if(SCANNER_DEBUG)  std::cerr << "Rozpoznałem teskt w komentarzu - " << yytext << std::endl;            }
[ \t]+  { if (SCANNER_DEBUG) std::cerr << "Rozpoznałem biały znak" << std::endl;   }
"VAR"     { if (SCANNER_DEBUG) std::cerr << "Rozpoznałem VAR" << std::endl;  return TOKEN_VAR; }
"BEGIN"   {  if (SCANNER_DEBUG) std::cerr << "Rozpoznałem BEGIN" << std::endl; return TOKEN_BEGIN; }
"END"     { if (SCANNER_DEBUG) std::cerr << "Rozpoznałem END" << std::endl;   return TOKEN_END; }
"ASSIGN"  {  if (SCANNER_DEBUG) std::cerr << "Rozpoznałem ASSIGN" << std::endl;  return ASSIGN; }
"PLUS"      {  if (SCANNER_DEBUG) std::cerr << "Rozpoznałem +" << std::endl;  return TOKEN_PLUS;                               }
"MINUS"      {  if (SCANNER_DEBUG) std::cerr << "Rozpoznałem -" << std::endl;  return TOKEN_MINUS;                               }
"TIMES"  {  if (SCANNER_DEBUG) std::cerr << "Rozpoznałem *" << std::endl;  return TOKEN_TIMES;                               }
"DIV"  {  if (SCANNER_DEBUG) std::cerr << "Rozpoznałem /" << std::endl;  return TOKEN_DIV;                               }
"MOD"  {  if (SCANNER_DEBUG) std::cerr << "Rozpoznałem %" << std::endl;  return TOKEN_MOD;                               }
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
"WHILE" { if (SCANNER_DEBUG) std::cerr << "Rozpoznałem WHILE" << std::endl; return TOKEN_WHILE; }
"DO" { if (SCANNER_DEBUG) std::cerr << "Rozpoznałem DO" << std::endl; return TOKEN_DO; }
"ENDWHILE" { if (SCANNER_DEBUG) std::cerr << "Rozpoznałem END_WHILE" << std::endl; return TOKEN_END_WHILE; }
"REPEAT" { if (SCANNER_DEBUG) std::cerr << "Rozpoznałem REPEAT" << std::endl; return TOKEN_REPEAT; }
"UNTIL" { if (SCANNER_DEBUG) std::cerr << "Rozpoznałem UNTIL" << std::endl; return TOKEN_UNTIL; }
"FOR" { if (SCANNER_DEBUG) std::cerr << "Rozpoznałem FOR" << std::endl; return TOKEN_FOR; }
"FROM" { if (SCANNER_DEBUG) std::cerr << "Rozpoznałem FROM" << std::endl; return TOKEN_FROM; }
"TO" { if (SCANNER_DEBUG) std::cerr << "Rozpoznałem TO" << std::endl; return TOKEN_TO; }
"DOWNTO"  { if (SCANNER_DEBUG) std::cerr << "Rozpoznałem DOWNTO" << std::endl; return TOKEN_DOWNTO; }
"ENDFOR" { if (SCANNER_DEBUG) std::cerr << "Rozpoznałem ENDFOR" << std::endl; return TOKEN_ENDFOR; }
[A-Za-z]+ {  if (SCANNER_DEBUG) std::cerr << "Rozpoznałem zmienną" << std::endl;  yylval.text = strdup(yytext); return pidentifier; }
-?[0-9]+  { if (SCANNER_DEBUG) std::cerr << "Rozpoznałem liczbę" << std::endl;  yylval.number = atoi(yytext);  return num; }
.       {  if (SCANNER_DEBUG) std::cerr << "Nie rozpoznałem tego symbolu " << " - " << yytext;   return TOKEN_ERROR;                              }


%%

int yywrap() {
    return 1;
}