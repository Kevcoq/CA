#include"continuations.h"

extern Value A;

extern Value ESCAPE;

extern Value ADD;

extern Value PRINT;

extern Value SET;

extern Continuation top_continuation;


/* Je n'ai pas reussi a faire marcher le call/cc, car il y a des problemes
 * avec les setjmp, longjmp.
 * Rapplee du main la fonction save_context() marche bien
 * mais depuis un autre fonction sa pose probleme...
 */

int main () {
  int test;
  Value B = make_integer(100);
  Continuation c;
  Value zubi;
  init();
  set_stack_properties(test);
 
  // call_cc(PRINT);
  
  save_context();
  printf("Hello\n");
  
  B = malloc(sizeof(struct Value));
  B->type = CONTINUATION_TAG;
  B->content.as_continuation = top_continuation;
  /* La fonction n'est jamais terminee */
  invoke0c(ESCAPE, B);
  return 0;
}
