#ifndef _VECTOR_T_
#define _VECTOR_T_

#include "base.h"

#define VEC_INI_SIZE 16
#define ALLOC_FACTOR  2

#define iterate(i, vec)\
    for (size_t i = 0; i < vec->size; ++i)

#define GENERATE_VECTOR_DEFINITION(TYPE, NAME)\
\
typedef struct\
{\
    TYPE* arr;\
    size_t size;\
    size_t capacity;\
} vec##NAME;\
\
vec##NAME* vec##NAME##_const()\
{\
    vec##NAME* v = s_calloc(sizeof(vec##NAME));\
    v->size = 0;\
    v->capacity = VEC_INI_SIZE;\
    v->arr = s_calloc(sizeof(TYPE)*v->capacity);\
    return v;\
}\
\
void vec##NAME##_dest(vec##NAME* v)\
{\
    S_FREE(v->arr);\
    S_FREE(v);\
}\
\
TYPE vec##NAME##_get(vec##NAME* v, size_t index)\
{\
    if (index >= v->size)\
    {\
        ERRPR("Index specified is out of bounds. Returned zero.");\
        return (TYPE)0;\
    }\
    return v->arr[index];\
}\
\
bool vec##NAME##_full(vec##NAME* v)\
{\
    return v->size >= v->capacity;\
}\
\
bool vec##NAME##_empty(vec##NAME* v)\
{\
    return v->size == 0;\
}\
\
void vec##NAME##_resize(vec##NAME* v)\
{\
    v->capacity *= ALLOC_FACTOR;\
    v->arr = s_realloc(v->arr, sizeof(TYPE)*v->capacity);\
}\
\
void vec##NAME##_set(vec##NAME* v, size_t index, TYPE value)\
{\
    if (index >= v->size)\
    {\
        ERRPR("Index specified is out of bounds. To increase size use \"vec##NAME_resize\" or \"vec##NAME_push_...\".");\
        return;\
    }\
    \
    v->arr[index] = value;\
}\
\
void vec##NAME##_push_back(vec##NAME* v, TYPE value)\
{\
    if (vec##NAME##_full(v))\
        vec##NAME##_resize(v);\
    v->size++;\
    v->arr[v->size-1] = value;\
}\
\
void vec##NAME##_push_front(vec##NAME* v, TYPE value)\
{\
    WRNPR("This function is very inefficient. Consider using \"vec##NAME_push_back\" variant instead.");\
    if (vec##NAME##_full(v))\
        vec##NAME##_resize(v);\
    for (size_t i = v->size; i > 0; --i)\
    {\
        v->arr[i] = v->arr[i-1];\
    }\
    v->arr[0] = value;\
    v->size++;\
}\
\
void vec##NAME##_print(vec##NAME* v)\
{\
    printf("Capacity: %ld, size: %ld\n", v->capacity, v->size);\
    iterate(i, v)\
    {\
        printf("%ld:\t", i);\
        GENERIC_DATA_PRINT(v->arr[i]);\
    }\
}

//for (size_t i = 0; i < v->size; ++i)
// {
//     printf("%ld: %d\n", i, v->arr[i]);
// }

#endif //_VECTOR_T_