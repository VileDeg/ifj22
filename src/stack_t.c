#include "stack_t.h"

#define STACK_INI_SIZE 16
#define ALLOC_FACTOR    2

struct stack
{
    stack_data_t* arr;
    size_t size;
    size_t capacity;
};

stack_t* stack_const()
{
    stack_t* st = s_calloc(sizeof(stack_t));
    st->size = 0;
    st->capacity = STACK_INI_SIZE;
    st->arr = s_calloc(sizeof(stack_data_t)*st->capacity);
    return st;
}

void stack_dest(stack_t* st)
{
    S_FREE(st->arr);
    S_FREE(st);
}

bool stack_full(stack_t* st)
{
    return st->size >= st->capacity;
}

bool stack_empty(stack_t* st)
{
    return st->size <= 0;
}

void stack_array_resize(stack_t* st)
{
    st->capacity = st->capacity * ALLOC_FACTOR;
    st->arr = s_realloc(st->arr, sizeof(stack_data_t)*st->capacity);
}

void stack_push(stack_t* st, stack_data_t d)
{
    if (stack_full(st))
        stack_array_resize(st);

    st->arr[st->size] = d;
    st->size++;
}

stack_data_t stack_pop(stack_t* st)
{
    if (stack_empty(st))
    {
        ERRPR("Stack is empty. Returned nothing.");
        return (stack_data_t)NULL;
    }
    st->size--;
    return st->arr[st->size];
}

stack_data_t stack_top(stack_t* st)
{
    if (stack_empty(st))
    {
        ERRPR("Stack is empty. Returned nothing.");
        return (stack_data_t)NULL;
    }
    return st->arr[st->size-1];
}