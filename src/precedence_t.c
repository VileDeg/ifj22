#include <stdio.h>
#include <stdlib.h>

#include "precedence_t.h"

void stack_init(symStack* stack) 
{
    stack->top = NULL;
}

void stack_push(symStack* stack, Oper_type item) 
{
    symbolPtr new = malloc(sizeof(Oper_type));      // TODO kontrola na NULL
    new->item = item;
    new->next = NULL;
    if(stack->top != NULL) {
        new->next = stack->top;
        stack->top = new;
    } else {
        stack->top = new;
    }
}

Oper_type stack_pop(symStack* stack) 
{
    Oper_type item;
    if(stack->top->next != NULL) {
        symbolPtr tmp = stack->top;
        item = tmp->item;
        stack->top = tmp->next;
        free(tmp);
    } else {
        item = stack->top->item;
        free(stack->top);
        stack->top = NULL;
    }
    return item;
}

void stack_clear(symStack* stack) 
{
    while(stack->top != NULL) {
        stack_pop(stack);
    }
}