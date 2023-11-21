/**
 * Project: Implementation of imperative language IFJ22 compiler.
 * 
 * @author Vadim Goncearenco (xgonce00@stud.fit.vutbr.cz)
 */

#ifndef __VECTOR_T__
#define __VECTOR_T__

/*Vector ADT(Abstract Data Type) for variable type.
  Implemented as DLL(Doubly-linked list). Is used mostly as stack.*/

#define GENERATE_VECTOR_DECLARATION(TDATA, PREF)\
\
typedef struct PREF##elem_t\
{\
    TDATA data;\
    struct PREF##elem_t* next;\
    struct PREF##elem_t* prev;\
} PREF##elem_t;\
\
typedef struct\
{\
    PREF##elem_t* front;\
    PREF##elem_t* back;\
} PREF##vec_t;\
\
void PREF##vec_init(PREF##vec_t* vec);\
TDATA PREF##vec_front(PREF##vec_t* vec);\
TDATA PREF##vec_back(PREF##vec_t* vec);\
bool PREF##vec_empty(PREF##vec_t* vec);\
bool PREF##vec_push_front(PREF##vec_t* vec, TDATA data);\
bool PREF##vec_push_back(PREF##vec_t* vec, TDATA data);\
TDATA PREF##vec_pop_front(PREF##vec_t* vec);\
TDATA PREF##vec_pop_back(PREF##vec_t* vec);\
void PREF##vec_dispose(PREF##vec_t* vec, void(*dest)(TDATA*));


#define GENERATE_VECTOR_DEFINITION(TDATA, PREF)\
\
void PREF##vec_init(PREF##vec_t* vec)\
{\
    vec->front = vec->back = NULL;\
}\
TDATA PREF##vec_front(PREF##vec_t* vec)\
{\
    return vec->front->data;\
}\
\
TDATA PREF##vec_back(PREF##vec_t* vec)\
{\
    return vec->back->data;\
}\
\
bool PREF##vec_empty(PREF##vec_t* vec)\
{\
    return vec->front == NULL && vec->back == NULL;\
}\
\
bool PREF##vec_push_front(PREF##vec_t* vec, TDATA data)\
{\
    PREF##elem_t* _new = calloc(1, sizeof(PREF##elem_t));\
    if (!_new) return false;\
    _new->data = data;\
    _new->prev = NULL;\
    _new->next = vec->front;\
    if (vec->front)\
        vec->front->prev = _new;\
    vec->front = _new;\
    if (!vec->back)\
        vec->back = vec->front;\
    return true;\
}\
\
bool PREF##vec_push_back(PREF##vec_t* vec, TDATA data)\
{\
    PREF##elem_t* _new = calloc(1, sizeof(PREF##elem_t));\
    if (!_new) return false;\
    _new->data = data;\
    _new->next = NULL;\
    _new->prev = vec->back;\
    if (vec->back)\
        vec->back->next = _new;\
    vec->back = _new;\
    if (!vec->front)\
        vec->front = vec->back;\
    return true;\
}\
\
TDATA PREF##vec_pop_front(PREF##vec_t* vec)\
{\
    PREF##elem_t* _tofree = vec->front;\
    TDATA data = _tofree->data;\
    if (vec->front->next)\
        vec->front->next->prev = NULL;\
    else\
        vec->back = NULL;\
    vec->front = vec->front->next;\
    free(_tofree);\
    return data;\
}\
\
TDATA PREF##vec_pop_back(PREF##vec_t* vec)\
{\
    PREF##elem_t* _tofree = vec->back;\
    TDATA data = _tofree->data;\
    if (vec->back->prev)\
        vec->back->prev->next = NULL;\
    else\
        vec->front = NULL;\
    vec->back = vec->back->prev;\
    free(_tofree);\
    return data;\
}\
\
void PREF##vec_dispose(PREF##vec_t* vec, void(*dest)(TDATA*))\
{\
    if (dest)\
        while (vec->front)\
        {\
            TDATA _tmp = PREF##vec_pop_front(vec);\
            dest(&_tmp);\
        }\
    else\
        while (vec->front)\
            PREF##vec_pop_front(vec);\
}

#endif // __VECTOR_T__
