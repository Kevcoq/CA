#include <stdio.h>
#include <stdlib.h>

#define malloc(n) mon_malloc()
#define free(n) /* mon_free(n) */

struct cons {int car; struct cons *cdr;};
typedef struct cons *liste_entier;

struct memoire
  {liste_entier liste_libre;
   unsigned int debut_tas;
   unsigned int fin_tas;
   int taille_tas;
   int taille_libre_tas;
   unsigned int debut_zone_statique;
   unsigned int fin_zone_statique;
   int sens_zone_statique;
   unsigned int debut_pile;
   int sens_pile;
   int nombre_GC;
};


typedef struct memoire *info_memoire;



extern info_memoire init_GC();
extern liste_entier mon_malloc();
/*extern void mon_free();*/
extern info_memoire GC;

#include "crible_gen.c"
liste_entier l_g;

int main_aux(int argc, char *argv[])
{liste_entier l1,l2;
 int x;
 if (argc != 3) 
  {fprintf(stderr,"mauvais nombre d'arguments\n"); exit (1);}
 else
 { init_GC(atoi(argv[1]));
   x = atoi(argv[2]); 
   l1 = eratosthene (x);
   printf("l1=");print_liste(l1);
   l_g = eratosthene(x);
   printf("l_g=");print_liste(l_g);
   printf("l1=");print_liste(l1);
   l2 = eratosthene(x);
   printf("l_g=");print_liste(l_g);
   printf("l1=");print_liste(l1);
   printf("l2=");print_liste(l2);
 }
}

