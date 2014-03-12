%{
#include <stdio.h>
#include "calc.tab.h"
%}

entier   [0-9]

%%


{entier}+        { yylval = atoi(yytext);
                   return ENT; }

"+"              { return PLUS; }
"-"              { return MOINS; }
"*"              { return FOIS; }
"/"              { return DIVISE; }
" "              { }
"\n"             { return FIN; }
"ans"            { return ANS; }
"^"              { return PUISS; }

%%

