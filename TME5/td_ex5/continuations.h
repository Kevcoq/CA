#include<stdio.h>
#include<stdlib.h>
#include<setjmp.h>

#define DECROISSANT       -1
#define CROISSANT          1

#define INTEGER_TAG       10
#define CONTINUATION_TAG  20
#define FUNCTION_TAG      30


typedef struct Value {
  char type;
  union {
    int as_int;
    char as_char;
    int * as_pointer;
    long as_long;
    struct Continuation * as_continuation;
    struct Function * as_function;
  } content;
} * Value;

typedef struct Continuation {
  jmp_buf registers;
  long * stack;
  long stack_height;
  struct Continuation * last;
} * Continuation; 

typedef struct Function {
  int arity;
  int arg_count;
  Value *  args;
  Value (* behaviour) ();
} * Function;

Value call_ep( Value function );

int SENS;

Continuation  top_continuation;
long * top_address;
Value jumpvalue;

Value make_integer (int x);
Value invoke (Value function, int n, Value * arguments );
Value call_cc(Value function);
Value invoke1 (Value function,  Value argument);
Value invoke1c (Value continuation, Value function,  Value argument);
Value invoke0c (Value continuation, Value function);
void set_stack_properties(int value);
