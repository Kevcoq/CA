%{
#include <stdio.h>
#include <math.h>

  int ans = 0;


  %}

%token ENT PLUS MOINS FOIS DIVISE FIN ANS PUISS
%left PLUS MOINS
%left FOIS DIVISE

%start Input

%%

Input : 
| Input Ligne
;

Ligne : FIN
| Exp FIN            {   ans = $1;
                         printf("Résultat : %d\n", $1);
                       }
;

Exp : ENT         { $$ = $1; }
| Exp Exp PLUS   { $$ = $1 + $2; }
| Exp Exp MOINS  { $$ = $1 - $2; }
| Exp Exp FOIS   { $$ = $1 * $2; }
| Exp Exp DIVISE { $$ = $1 / $2; }
| ANS            { $$ = ans; }
| Exp Exp PUISS  { $$ = (int) pow($1, $2); }
;

%%

int yyerror (char *s) {
  printf("%s\n", s);
} 

int main () {
  yyparse();
}
