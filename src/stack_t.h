#ifndef _STACK_H_
#define _STACK_H_

#include "base.h"

static void print_int(int x)   { printf("int: %d\n", x); }
static void print_float(float x) { printf("float: %g\n", x); }
static void print_default()        { ERRPR("Unsupported type."); }

#define STACK_DATA_PRINT(data) _Generic((data), \
        int:     print_int, \
        float:   print_float, \
        default: print_default)(data)

#define STACK_INI_SIZE 16
#define ALLOC_FACTOR    2

#define GENERATE_STACK(TYPE, NAME)\
\
typedef struct\
{\
    TYPE *arr;\
    size_t size;\
    size_t capacity;\
} stack_##NAME;\
\
stack_##NAME *stack_const_##NAME()\
{\
    stack_##NAME *st = s_calloc(sizeof(stack_##NAME));\
    st->size = 0;\
    st->capacity = STACK_INI_SIZE;\
    st->arr = s_calloc(sizeof(TYPE) * st->capacity);\
    return st;\
}\
\
void stack_dest_##NAME(stack_##NAME *st)\
{\
    S_FREE(st->arr);\
    S_FREE(st);\
}\
\
bool stack_full_##NAME(stack_##NAME *st)\
{\
    return st->size >= st->capacity;\
}\
\
bool stack_empty_##NAME(stack_##NAME *st)\
{\
    return st->size <= 0;\
}\
\
void stack_array_resize_##NAME(stack_##NAME *st)\
{\
    st->capacity = st->capacity * ALLOC_FACTOR;\
    st->arr = s_realloc(st->arr, sizeof(TYPE) * st->capacity);\
}\
\
void stack_push_##NAME(stack_##NAME *st, TYPE d)\
{\
    if (stack_full_##NAME(st))\
        stack_array_resize_##NAME(st);\
\
    st->arr[st->size] = d;\
    st->size++;\
}\
\
TYPE stack_pop_##NAME(stack_##NAME *st)\
{\
    if (stack_empty_##NAME(st))\
    {\
        ERRPR("Stack is empty. Returned nothing.");\
        return (TYPE)0;\
    }\
    st->size--;\
    return st->arr[st->size];\
}\
\
TYPE stack_top_##NAME(stack_##NAME *st)\
{\
    if (stack_empty_##NAME(st))\
    {\
        ERRPR("Stack is empty. Returned nothing.");\
        return (TYPE)0;\
    }\
    return st->arr[st->size - 1];\
}\
\
void stack_print_##NAME(stack_##NAME *st)\
{\
    for (int i = 0; !stack_empty_##NAME(st); ++i)\
    {\
        printf("%d:\t", i);\
        STACK_DATA_PRINT(stack_pop_##NAME(st));\
    }\
}\

#endif //_STACK_H_