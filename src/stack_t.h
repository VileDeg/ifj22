#ifndef _STACK_H_
#define _STACK_H_

#include "base.h"

#define STACK_INI_SIZE 16
#define ALLOC_FACTOR    2

#define GENERATE_STACK_DEFINITION(TYPE, NAME)\
\
typedef struct\
{\
    TYPE *arr;\
    size_t size;\
    size_t capacity;\
} stack##NAME;\
\
stack##NAME *stack##NAME##_const()\
{\
    stack##NAME *st = s_calloc(sizeof(stack##NAME));\
    st->size = 0;\
    st->capacity = STACK_INI_SIZE;\
    st->arr = s_calloc(sizeof(TYPE) * st->capacity);\
    return st;\
}\
\
void stack##NAME##_dest(stack##NAME *st)\
{\
    S_FREE(st->arr);\
    S_FREE(st);\
}\
\
bool stack##NAME##_full(stack##NAME *st)\
{\
    return st->size >= st->capacity;\
}\
\
bool stack##NAME##_empty(stack##NAME *st)\
{\
    return st->size <= 0;\
}\
\
void stack##NAME##_resize(stack##NAME *st)\
{\
    st->capacity = st->capacity * ALLOC_FACTOR;\
    st->arr = s_realloc(st->arr, sizeof(TYPE) * st->capacity);\
}\
\
void stack##NAME##_push(stack##NAME *st, TYPE d)\
{\
    if (stack##NAME##_full(st))\
        stack##NAME##_resize(st);\
\
    st->arr[st->size] = d;\
    st->size++;\
}\
\
TYPE stack##NAME##_pop(stack##NAME *st)\
{\
    if (stack##NAME##_empty(st))\
    {\
        ERRPR("Stack is empty. Returned nothing.");\
        return (TYPE)0;\
    }\
    st->size--;\
    return st->arr[st->size];\
}\
\
TYPE stack##NAME##_top(stack##NAME *st)\
{\
    if (stack##NAME##_empty(st))\
    {\
        ERRPR("Stack is empty. Returned nothing.");\
        return (TYPE)0;\
    }\
    return st->arr[st->size - 1];\
}\
\
void stack##NAME##_print(stack##NAME *st)\
{\
    for (int i = 0; !stack##NAME##_empty(st); ++i)\
    {\
        printf("%d:\t", i);\
        GENERIC_DATA_PRINT(stack##NAME##_pop(st));\
    }\
}

#endif //_STACK_H_