#include <stdio.h>
#include <stdlib.h>



struct cons {int car; struct cons *cdr;};
typedef struct cons *liste_entier;

#define size_cons (sizeof(struct cons))

liste_entier liste_libre;

liste_entier init_alloc(int n)
{liste_entier l;
 int i;
 liste_libre=(liste_entier)(malloc(n*size_cons));
 l=liste_libre;
 for (i=0; i<(n-1); i++)
  { l->cdr=(liste_entier)((int)(l)+size_cons);
    l=l->cdr;
  }
 l->cdr=NULL;
 return liste_libre;
}

liste_entier mon_malloc ()
{liste_entier l;
  if (liste_libre == NULL) {fprintf (stderr,"Plus d'espace\n");exit (-1);}
  else 
  { l=liste_libre;
    liste_libre=liste_libre->cdr;
    return l;
  }
}

void mon_free (liste_entier l)
{liste_entier l_aux;
 l->cdr=liste_libre;
 liste_libre=l;
}

