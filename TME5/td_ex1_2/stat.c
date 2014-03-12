
extern char *etext, *edata, *end;
int main();

void zone_statique()
{
  if (((int)&etext) < ((int)&main))
       printf("debut = %d fin = %d sens = %d\n",&end,&edata,1);
  else printf("debut = %d fin = %d sens = %d\n",&end,&edata,-1);
}

int i;
int main()
{static int j;
 zone_statique();
}
