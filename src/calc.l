%{
#include <iostream>
#include <string>
#include "calc_y.h"

int yylex();

%}

%%
[ \t] ;
^#.*\n ;
[0-9]+  { yylval = std::atoi(yytext); return NUMBER; }
\+      { return PLUS;                       }
\-      { return MINUS;                          }
\*      { return MULTI;                             }
\/      { return DIV;                               }
\(      { return LNAW;                              }
\)      { return PNAW;                              }
\n      { return END; }
.       { return ERROR;                            }
%%

int yywrap() {
    return 1;
}