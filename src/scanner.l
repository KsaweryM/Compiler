%{
#include <iostream>
#include <string>
#include "parser_y.h"

int yylex();
%}

%%
[ \t] ;
^#.*\n ;
"VAR"     { return VAR; }
"BEGIN"   { return BEGIN; }
"END"     { return END; }
\+      { return TOKEN_PLUS;                               }
\-      { return TOKEN_MINUS;                              }
\[      { return TOKEN_LNAW;                               }
\]      { return TOKEN_PNAW;                               }
.       { return TOKEN_ERROR;                              }
%%

int yywrap() {
    return 1;
}