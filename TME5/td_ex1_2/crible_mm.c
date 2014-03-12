#include <stdio.h>
#include <stdlib.h>

#define malloc(n) mon_malloc()
#define free(n) mon_free(n)

struct cons {int car; struct cons *cdr;};
typedef struct cons *liste_entier;


extern liste_entier init_alloc();
extern liste_entier mon_malloc();
extern void mon_free();


#include "crible_gen.c"

main(int argc, char *argv[])
{liste_entier l;
 int x;
 if (argc != 3) 
  {fprintf(stderr,"mauvais nombre d'arguments\n"); exit (1);}
 else
 { init_alloc(atoi(argv[1]));
   x = atoi(argv[2]); 
   l = eratosthene (x);
   print_liste(l);
 }
}

