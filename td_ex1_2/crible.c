#include <stdio.h>
#include <stdlib.h>

struct cons {int car; struct cons *cdr;};
typedef struct cons *liste_entier;

#include "crible_gen.c"

main(int argc, char *argv[])
{liste_entier l;
 int x;
 if (argc == 1) 
  {fprintf(stderr,"pas d'arguments\n"); exit (1);}
 else
 { x = atoi(argv[1]); 
   l = eratosthene (x);
   print_liste(l);
 }
}

