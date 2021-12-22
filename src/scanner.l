%{
#include <iostream>
#include <string>
#include "../inc/Commands/commands.h"
#include "parser_y.h"

int yylex();
%}

%%
[ \t] ;
^#.*\n ;
[0-9]+  { yylval.number = std::atoi(yytext); return TOKEN_NUMBER; }
\+      { return TOKEN_PLUS;                               }
\-      { return TOKEN_MINUS;                              }
\*      { return TOKEN_MULTI;                              }
\/      { return TOKEN_DIV;                                }
\(      { return TOKEN_LNAW;                               }
\)      { return TOKEN_PNAW;                               }
\n     { return TOKEN_END;                                }
.       { return TOKEN_ERROR;                              }
%%

int yywrap() {
    return 1;
}