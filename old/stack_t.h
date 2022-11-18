#ifndef __STACK_H__
#define __STACK_H__

#include <stdlib.h>
#include <stdbool.h>

#define GENERATE_STACK_DECLARATION(TDATA, SUFF)\
\
typedef struct SUFF##elem_t\
{\
    TDATA data;\
    struct SUFF##elem_t* next;\
} SUFF##elem_t;\
\
typedef struct SUFF##stack_t\
{\
    SUFF##elem_t* top;\
} SUFF##stack_t;\
\
void SUFF##stack_init(SUFF##stack_t* stack); \
\
bool SUFF##stack_push(SUFF##stack_t* stack, TDATA data); \
\
void SUFF##stack_pop(SUFF##stack_t* stack); \
\
TDATA SUFF##stack_top(SUFF##stack_t* stack);\
\
bool SUFF##stack_empty(SUFF##stack_t* stack);\
\
void SUFF##stack_dispose(SUFF##stack_t* stack);


#define GENERATE_STACK_DEFINITION(TDATA, SUFF)\
\
typedef struct SUFF##elem_t\
{\
    TDATA data;\
    struct SUFF##elem_t* next;\
} SUFF##elem_t;\
\
typedef struct SUFF##stack_t\
{\
    SUFF##elem_t* top;\
} SUFF##stack_t;\
\
void SUFF##stack_init(SUFF##stack_t* stack) \
{\
    stack->top = NULL;\
}\
\
bool SUFF##stack_push(SUFF##stack_t* stack, TDATA data) \
{\
    SUFF##elem_t *new = malloc(sizeof(SUFF##elem_t));\
    if (!new)\
        return false;\
    new->data = data;\
    new->next = stack->top;\
    stack->top = new;\
}\
\
void SUFF##stack_pop(SUFF##stack_t* stack) \
{\
    if (!stack->top)\
        return;\
    SUFF##elem_t* tofree = stack->top;\
    stack->top = stack->top->next;\
    free(tofree);\
}\
\
TDATA SUFF##stack_top(SUFF##stack_t* stack)\
{\
    return stack->top->data;\
}\
\
bool SUFF##stack_empty(SUFF##stack_t* stack)\
{\
    return !stack->top;\
}\
\
void SUFF##stack_dispose(SUFF##stack_t* stack)\
{\
    while (stack->top)\
    {\
        SUFF##elem_t* tofree = stack->top;\
        stack->top = stack->top->next;\
        free(tofree);\
    }\
}

#endif //__STACK_H__