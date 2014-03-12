liste_entier cons(int a, liste_entier l)
{liste_entier r;
 r=(liste_entier)(malloc(sizeof(struct cons)));
 r->car=a;
 r->cdr=l;
 return r;
}

liste_entier intervalle(int a, int b)
{ if (a > b) return NULL;
  else return cons(a,intervalle(a+1,b));
}

liste_entier filtre_mult( int n,liste_entier l)
{
  if (l == NULL) return NULL;
  else 
  { liste_entier l_aux;
    int i_aux;
    l_aux = l->cdr;
    i_aux = l->car;
    if ((i_aux % n) != 0) {free(l); return cons(i_aux,filtre_mult(n,l_aux));}
    else {free(l); return filtre_mult(n,l_aux);}
  } 
}

void print_liste();

liste_entier era_aux (liste_entier l1, liste_entier l2)
{ int a;
/*
  printf("l1 = %d ",&l1);print_liste(l1);printf("\n");
  printf("l2 = %d ",&l2);print_liste(l2);printf("\n\n");
*/

  if (l2 == NULL) return l1;
  else 
   {a=l2->car;
    return era_aux (cons (a, l1),filtre_mult(a,l2));
   }
} 

liste_entier eratosthene(int n)
{
 return era_aux(NULL, intervalle(2,n));
}
 
void print_liste(liste_entier l)
{
  printf("[ "); 
  while (l) 
   {printf("%d ",l->car);
    l=l->cdr;
   }
  printf("]\n");
}

