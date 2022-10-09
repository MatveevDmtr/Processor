#ifndef STACK_CONFIG_H_INCLUDED
#define STACK_CONFIG_H_INCLUDED

#include "stack.h"

#define USER_MODE      0
#define VERIFIER_MODE  1
#define HASH_MODE      2
#define HARDDEBUG_MODE 3

#define STACK_MODE 3

typedef int elem_t;

const int INT_CONST = 300;

const int MIN_LEN_STACK = 10;

#endif

