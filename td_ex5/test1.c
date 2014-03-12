#include"continuations.h"

extern Value A;

extern Value ESCAPE;

extern Value ADD;

extern Value PRINT;

extern Value SET;


/* Un test qui additionne simplement deux chiffres
 *
 */
void my_main () {
  Value a, b;
  a =  make_integer(4);
  b =  make_integer(6);
  printf("TEST 1 : continuations \n");
  invoke0c (ESCAPE, invoke0c(PRINT, invoke1(invoke1(ADD, b), a)));
}

int main () {
  int test;
  init();
  set_stack_properties(test);
  my_main();
  invoke0c(ESCAPE, A);
  return 0;
}
