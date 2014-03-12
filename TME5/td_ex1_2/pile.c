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

unsigned int debut;

void affiche ()
 {unsigned int fin;
  {int a,b,c,d,e,f;
   a=45;
   b = a +17;
   fin=adresse_pile(b);
   printf("debut_pile = %u fin_pile=%u sens_pile=%d\n",
           debut,fin,sens_pile());
  }
 }

main()
{ debut=adresse_pile(5);
  affiche();
}
