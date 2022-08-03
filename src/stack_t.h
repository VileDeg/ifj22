#ifndef _STACK_H_
#define _STACK_H_

#include "base.h"

#define stack_data_t int

struct stack;
typedef struct stack stack_t;

stack_t* stack_const();

void stack_dest(stack_t* st);

bool stack_empty(stack_t* st);

void stack_push(stack_t* st, stack_data_t d);

stack_data_t stack_pop(stack_t* st);

stack_data_t stack_top(stack_t* st);

#endif //_STACK_H_