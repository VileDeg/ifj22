#ifndef __VECTOR_T__
#define __VECTOR_T__

#define GENERATE_VECTOR_DECLARATION(TDATA, SUFF)\
\
typedef struct SUFF##elem_t\
{\
    TDATA data;\
    struct SUFF##elem_t* next;\
    struct SUFF##elem_t* prev;\
} SUFF##elem_t;\
\
typedef struct\
{\
    SUFF##elem_t* front;\
    SUFF##elem_t* back;\
    int64_t len;\
} SUFF##vec_t;\
\
void SUFF##vec_init(SUFF##vec_t* vec);\
TDATA SUFF##vec_front(SUFF##vec_t* vec);\
TDATA SUFF##vec_back(SUFF##vec_t* vec);\
bool SUFF##vec_empty(SUFF##vec_t* vec);\
bool SUFF##vec_push_front(SUFF##vec_t* vec, TDATA data);\
bool SUFF##vec_push_back(SUFF##vec_t* vec, TDATA data);\
TDATA SUFF##vec_pop_front(SUFF##vec_t* vec);\
TDATA SUFF##vec_pop_back(SUFF##vec_t* vec);\
TDATA SUFF##vec_extract(SUFF##vec_t* vec, SUFF##elem_t* elem);\
void SUFF##vec_dispose(SUFF##vec_t* vec, void(*dest)(TDATA*));

#define GENERATE_VECTOR_DEFINITION(TDATA, SUFF)\
\
void SUFF##vec_init(SUFF##vec_t* vec)\
{\
    vec->front = vec->back = NULL;\
    vec->len = 0;\
}\
TDATA SUFF##vec_front(SUFF##vec_t* vec)\
{\
    return vec->front->data;\
}\
\
TDATA SUFF##vec_back(SUFF##vec_t* vec)\
{\
    return vec->back->data;\
}\
\
bool SUFF##vec_empty(SUFF##vec_t* vec)\
{\
    return vec->front == NULL && vec->back == NULL;\
}\
\
bool SUFF##vec_push_front(SUFF##vec_t* vec, TDATA data)\
{\
    SUFF##elem_t* _new = calloc(1, sizeof(SUFF##elem_t));\
    if (!_new) return false;\
    _new->data = data;\
    _new->next = NULL;\
    _new->prev = vec->front;\
    if (vec->front)\
        vec->front->next = _new;\
    vec->front = _new;\
    if (!vec->back)\
        vec->back = vec->front;\
    vec->len++;\
    return true;\
}\
\
bool SUFF##vec_push_back(SUFF##vec_t* vec, TDATA data)\
{\
    SUFF##elem_t* _new = calloc(1, sizeof(SUFF##elem_t));\
    if (!_new) return false;\
    _new->data = data;\
    _new->prev = NULL;\
    _new->next = vec->back;\
    if (vec->back)\
        vec->back->prev = _new;\
    vec->back = _new;\
    if (!vec->front)\
        vec->front = vec->back;\
    vec->len++;\
    return true;\
}\
\
TDATA SUFF##vec_pop_front(SUFF##vec_t* vec)\
{\
    SUFF##elem_t* _tofree = vec->front;\
    TDATA data = _tofree->data;\
    if (vec->front->prev)\
        vec->front->prev->next = NULL;\
    else\
        vec->back = NULL;\
    vec->front = vec->front->prev;\
    free(_tofree);\
    vec->len--;\
    return data;\
}\
\
TDATA SUFF##vec_pop_back(SUFF##vec_t* vec)\
{\
    SUFF##elem_t* _tofree = vec->back;\
    TDATA data = _tofree->data;\
    if (vec->back->next)\
        vec->back->next->prev = NULL;\
    else\
        vec->front = NULL;\
    vec->back = vec->back->next;\
    free(_tofree);\
    vec->len--;\
    return data;\
}\
\
TDATA SUFF##vec_extract(SUFF##vec_t* vec, SUFF##elem_t* elem)\
{\
    if (elem->prev)\
        elem->prev->next = elem->next;\
    else\
        vec->back = elem->next;\
    if (elem->next)\
        elem->next->prev = elem->prev;\
    else\
        vec->front = elem->prev;\
    TDATA _data = elem->data;\
    free(elem);\
    vec->len--;\
    return _data;\
}\
\
void SUFF##vec_dispose(SUFF##vec_t* vec, void(*dest)(TDATA*))\
{\
    if (dest)\
        while (vec->front)\
        {\
            TDATA _tmp = SUFF##vec_pop_front(vec);\
            dest(&_tmp);\
        }\
    else\
        while (vec->front)\
            SUFF##vec_pop_front(vec);\
}

#endif // __VECTOR_T__
