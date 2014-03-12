
#include <stdio.h>
#include <stdlib.h>



struct cons {int car; struct cons *cdr;};
typedef struct cons *liste_entier;

extern liste_entier l_g;

#define size_cons (sizeof(struct cons))
#define size_int  (sizeof(int))
#define size_liste_entier (sizeof(liste_entier))

extern char *etext, *edata, *end;
extern int main();

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


unsigned int adresse_pile(x)
unsigned int x;
{return (unsigned int)&x;}

int sens_pile ()
{unsigned int a;
 unsigned int b;
 b = adresse_pile(3);
/*
 printf(" b=%u a=%u b-a=%d\n",b,(unsigned int)&a,b-(unsigned int)&a);
*/
 if (((int)(b-(unsigned int)(&a))) > 0) {return 1;}
 else return (-1);
}

unsigned int debut_pile;
info_memoire GC;

info_memoire init_GC(int n)
{liste_entier l;
 int i;
 if (size_int != size_liste_entier) 
   {fprintf(stderr,"Arret du programme,taille(int)=%d != taille(pointeur)=%d\n"
                  ,size_int,size_liste_entier);
    exit (-1);
   }
 GC=(info_memoire)(malloc(sizeof(struct memoire)));
 GC->liste_libre=(liste_entier)(malloc(n*size_cons));
 GC->taille_tas=n;
 GC->taille_libre_tas=n;
 GC->debut_tas=(int)GC->liste_libre;
 GC->fin_tas=((int)GC->liste_libre)+(n*size_cons);
 GC->sens_pile=sens_pile();
 GC->debut_pile=debut_pile;
 GC->debut_zone_statique=(unsigned)&end;
 GC->fin_zone_statique=(unsigned)&edata;
 GC->sens_zone_statique=
   (GC->debut_zone_statique < GC->fin_zone_statique) ? 1 : (-1);
 l=GC->liste_libre;
 /*printf("avant parcours du tas\n");*/
 for (i=0; i<(n-1) ;i++)
  { l->cdr=(liste_entier)((int)(l)+size_cons);
    l=l->cdr;
  }
/* printf("apres parcours du tas\n");*/
 l->cdr=NULL;
 l=GC->liste_libre;
/* for (i=0; i<(n-1) ;i++)
  { printf("%d . ",(int)l);
    l=l->cdr;
  }
*/
 GC->nombre_GC=0;
 return GC;
}

int pointeur_correct (liste_entier l)
{
  if ( (((int)l) < GC->debut_tas) || (((int) l) >= (GC->fin_tas))) return 0;
  else if ( (((int) l) % size_int) != 0) return 0;
       else return 1;
}

#define MASQUE 1
#define est_marque(l) (( (int)l) &  MASQUE)
#define marque(l)  (l=((liste_entier)(((int)l) | MASQUE)))
#define demarque(l) (l=((liste_entier)(((int)l) ^ MASQUE)))

void mark(liste_entier l)
{ liste_entier l_aux;
/*  printf("m=%d .. ",(int)l);*/
  
  while  (pointeur_correct(l))
        { /*printf("marque...\n");*/
          l_aux=l->cdr;
          marque(l->cdr);
          l=l_aux;
        }
}


int sweep ()
{ liste_entier l_aux;
  int compteur=0;
  int i;

  for (i=(int)(GC->debut_tas);  i< (int)(GC->fin_tas); i=i+size_cons)
  { if (est_marque(((liste_entier)i)->cdr)) demarque(((liste_entier)i)->cdr);
    else
      { l_aux=(liste_entier)i;
        l_aux->cdr=GC->liste_libre;
        GC->liste_libre=l_aux;
        compteur++;
      }
  }
/*  printf("GC = %d recup \n",compteur);*/
  return compteur;
}

int gc()
{ int i;
  int fin_pile;
  int r;

/*
  printf ("debut de gc\n");
*/
/*  printf("d=%d f=%d s=%d\n",(int)GC->debut_zone_statique,
          (int)GC->fin_zone_statique,GC->sens_zone_statique);
  printf("&l_g=%d &main=%d\n",(int)(&l_g),(int)(&main));
*/
  for (i=(int)GC->debut_zone_statique;
       i != ((int)GC->fin_zone_statique);
       i=i+ size_int*GC->sens_zone_statique) 
         { /*if (i == (int)(&(l_g))) {printf("OK");}*/
           mark( (liste_entier)(*((int *)i)) );}
  
  fin_pile=adresse_pile(18);

/*  printf("deb = %d, fin = %d, sens = %d\n", 
         GC->debut_pile, fin_pile, GC->sens_pile);
*/
  for (i=(int)GC->debut_pile;
       i != fin_pile;
       i=i+ size_int*GC->sens_pile)  { mark( (liste_entier)(*((int *)i)) ); }

  r=sweep();
  GC->nombre_GC++;
  GC->taille_libre_tas=r;
#ifdef TRACE
  printf("Apres GC numero %d, %d cellules sont libres\n",
         GC->nombre_GC,GC->taille_libre_tas);
#endif
  return (r);
}

liste_entier mon_malloc()
{liste_entier l;
/*  printf ("fl = %d\n",GC->liste_libre);*/
  if (GC->liste_libre == NULL) 
    { if (gc()) return mon_malloc();
      else {fprintf (stderr,"Plus d'espace\n");exit (-1);}
    }
  else
  { l=GC->liste_libre;
    GC->liste_libre=GC->liste_libre->cdr;
    return l;
  }
}

extern int main_aux();

int main(int argc, char *argv[])
{ debut_pile = adresse_pile(3);
  main_aux(argc,argv);
}

