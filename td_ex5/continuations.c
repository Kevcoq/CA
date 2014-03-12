#include "continuations.h"


/*
 * Itercale deux tableaux -> pour les listes d'arguments
 */
Value * shift(Value * t1, int s1, Value * t2, int s2){
  Value * new = (Value *) malloc(sizeof(struct Value)*(s1+s2));
  int i;

  if(s1==0)
    return t2;
  if(s2==0)
    return t1;

  for(i=0; i<s1 ;i++){
    new[i]=t1[i];
  }

  for(; i<s1+s2; i++){
    new[i]=t2[i-s1];
  }
  return new;
    
}
/* Emballe un entier dans une structure Value
 * pour simplifier j'ai utilise uniquement des valuers entieres dans les tests
 */
Value make_integer(int x){
  Value new_integer = malloc(sizeof(struct Value));
  new_integer->type = INTEGER_TAG;
  new_integer->content.as_int = x;
  return new_integer;
}

long * allocate_adress () {
  long i;
  long j = (long)&i;
  return (long *)j;
}

/* verifie si le sens de la pile est croissant ou decroissant 
 *  et sauvegarde l'adresse de la tete de la pile 
 */
void set_stack_properties (int adr) {
  long i, * j;
  top_continuation=NULL;
  j=allocate_adress();
  if((&i - j) < 0)
    SENS = CROISSANT;
  else 
    SENS = DECROISSANT;
  top_address = &adr; 
} 

/* 
 * Sauvegarde la pile et les registres -> cree un objet continuation
 */
Continuation  save_context () {
  Continuation  c = malloc(sizeof(struct Continuation));
  long address, taille; 
  int val;
  int i;
  //printf("Saving registers\n");
  taille = (&address - top_address)*SENS;
  //printf("Saving stack : taille  %d \n", taille);
  c->stack = (long *)malloc(taille*sizeof(long));
  c->stack_height = taille;
 
  if((val=setjmp(c->registers)) == 0){
    for(i=0; i<taille; ++i){
      c->stack[i] = top_address[SENS*i];  
      //printf("Saving stack          %d   %ld %d\n ", i, c->stack[i], top_address);
    }
    //printf("---------> REGISTRES %d \n", c->registers);
    c->last = top_continuation;
    top_continuation = c;
    return c;
  }
  //printf("SETJMP RETOUR NON 0 %d \n", val);
  //printf("Hauteur de la pile :  %d \n", c->stack_height);
  return NULL;
}

void get_context2 (Continuation c, int once_more){
  long padding [10];
  long i;
  // printf("DEBUG0 %ld  %d\n", (long)(top_address-&i), c->stack_height);
  if(once_more) {
    if ((SENS == DECROISSANT) && ((top_address - &i) < c->stack_height))
      get_context2(c, 1);
    else if ((SENS == CROISSANT)&&((top_address - c->stack_height) > &i))
      get_context2(c, 1);
    else 
      get_context2(c, 0);
  }
  //printf("DEBUG3  \n");
  
  for(i=0; i<c->stack_height; ++i){
    top_address[-i]=c->stack[i];
    //printf(" Restroring registers          %d   %ld %d\n", i, top_address[-i], top_address);
  }
  //printf("DEBUG4 %d %ld\n", c->registers, (long)&i);
  
  longjmp(c->registers, 0);

  //printf("DEBUG5  \n");
 }

/*
 * Restore la pile et les registres
 */
void get_context (Continuation c){
  get_context2(c, 1);
}

/*
 * invoke - prend une fonction et des arguments et retourne en fonction 
 * du type de la fonction et du nombre d'arguments soit une nouvelle fermeture
 * soit un application de la fonction
 * soit restaure le contexte d'une continuation
 */
Value invoke (Value function, int n, Value * arguments ) {
  //printf("Entre dans invoke : \n");
  switch (function->type){
    
  case FUNCTION_TAG :{
    long arity = function->content.as_function->arity;
    long arg_count = function->content.as_function->arg_count;
    Value (* behaviour)(void) = function->content.as_function->behaviour;
    Value *  args = function->content.as_function->args;
    int i;
    
    if(arity == arg_count + n){
     
      /* on a le bon nombre d'arguments -> on peut appliquer la fonction
       * on considere que les fonctions de moins de 4 arguments 
       */
      args = shift(args, arg_count, arguments, n);
   
      switch(arity){
	Value  result;
      case 1:
	result = ((Value (*)(Value)) * behaviour)(args[0]);
      case 2:
	result = ((Value (*)(Value, Value)) * behaviour) (args[0], args[1]);
      case 3:
	result = ((Value (*)(Value, Value, Value)) * behaviour)
	  (args[0], args[1], args[2]);

	return result;
      }
    } else if(arity>arg_count + n){
      /* Le nombre d'arguments est insuffisant -> on sauvegarde le nouveau environnement
       * et on cree une nouvelle fonction avec un arite inferieure
       */
    
      Value new_function = (Value)malloc(sizeof(struct Value));
      Function content = (Function)malloc(sizeof(struct Function));
      content->arity = arity;
      content->arg_count = arg_count + n;
      content->args = shift(args, arg_count, arguments, n);
      content->behaviour = behaviour;
      new_function->type = FUNCTION_TAG;
      new_function->content.as_function = content;
      return new_function;
    } else {
      /*
       * Trop d'arguments impossible appliquer la fonction
       */
      printf("ERROR: TOO MANY ARGUMENTS \n");
      return NULL;
    }
  }
    
    /* L'invocation d'une continuation restaure tout simplement le contexte
     * qu'elle referme
     */
  case CONTINUATION_TAG : 
    if(n==1) {
      get_context(function->content.as_continuation);
    } else {
      printf("ERROR : Too many arguments");
      return NULL;
    }
  default :
    break;
    printf("Ne peut pas s'appliquer");
  }
  return NULL;
}

/* Par soucis esthetique, permet d'invoquer la fonction precedee par sa continuation  
 * et reorganise le nombre d'arguments
 */
Value invoke_with_continuation (Value continuation, 
				Value function,
				int n,
				Value * arguments) {
  
  Value * args;
  Value * table = malloc(sizeof(Value));
  table[0] = continuation;
  args = shift(arguments, n, table, 1);
  return invoke(function, n+1, args);
}

/* Des raccourcis pour appler invoke avec 0 ou 1 arguments et/ou une continuation
 *
 */
Value invoke1 (Value function,  Value argument) {
  Value * table = malloc(sizeof(Value));
  table[0] = argument;
  return invoke(function, 1, table);
}
Value invoke1c (Value continuation, Value function,  Value argument) {
  Value * table = malloc(sizeof(Value));
  table[0] = argument;
  return invoke_with_continuation (continuation, function, 1, table);
}
Value invoke0c (Value continuation, Value function) {
  return invoke_with_continuation (continuation, function, 0, NULL);
}




/* Prend en argument une fonction qui a une arite de 1
 * Capture la continuation courante dans une valeur
 * 
 */
Value call_cc(Value function) {
  /*
   *  Capture la continuation et l' "emballe" (reify)  dans une Valeur
   */
  Value k = (Value)malloc(sizeof(struct Value));
  k->type=CONTINUATION_TAG;
  k->content.as_continuation = save_context();
  return invoke1(function, k);
}


