#include"continuations.h"


/* Des definitions des fonctions
 * Pour les tests
 */
Value ESCAPE;

Value ADD;

Value PRINT;

Value SET;

Value A;

Value escape (Value terminate){
  printf("ESCAPE CALLED\n");
  exit(terminate->content.as_int);
}

Value add (Value a, Value b, Value continuation){
  Value sum;
  int i = a->content.as_int + b->content.as_int;
  sum = (Value)make_integer(i);
  return (Value)invoke1(continuation, sum);
}


Value print (Value a, Value continuation){
  if(a->type == INTEGER_TAG) {
     printf("Your data is an Integer: %d \n", a->content.as_int);
  } else
    printf("Your data : %d \n", a->type);
  return (Value)invoke1(continuation, a);
}


Value set (Value a, Value b, Value continuation) {
 
  printf("ENTERED SET %d  %d\n", a->type, b->type);
  A = b;
  a = b;
  
  printf("ENTERED SET %d  %d\n", a->type, b->type);
  
  return (Value)invoke1(continuation, a);
}

/* Initialisation des fonctions
 *
 */
void init () {
  Function f;
  ADD = malloc(sizeof(struct Value));
  f = malloc(sizeof(struct Function));
  f->arity=3;
  f->arg_count=0;
  f->args=NULL;
  f->behaviour=add;

   ADD->type = FUNCTION_TAG;
  ADD->content.as_function = f;

  PRINT = malloc(sizeof(struct Value)+sizeof(struct Function));
  f = malloc(sizeof(struct Function));
  f->arity=2;
  f->arg_count=0;
  f->args=NULL;
  f->behaviour=print;

  PRINT->type = FUNCTION_TAG;
  PRINT->content.as_function = f;

  ESCAPE = malloc(sizeof(struct Value)+sizeof(struct Function));
  f = malloc(sizeof(struct Function));
  f->arity=1;
  f->arg_count=0;
  f->args=NULL;
  f->behaviour=escape;

  ESCAPE->type = FUNCTION_TAG;
  ESCAPE->content.as_function = f;

  SET = malloc(sizeof(struct Value)+sizeof(struct Function));
  f = malloc(sizeof(struct Function));
  f->arity=3;
  f->arg_count=0;
  f->args=NULL;
  f->behaviour=set;

  SET->type = FUNCTION_TAG;
  SET->content.as_function = f;

}
