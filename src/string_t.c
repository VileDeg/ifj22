/**
 * Project: Implementation of imperative language IFJ22 compiler.
 * 
 * @author Vadim Goncearenco  (xgonce00@stud.fit.vutbr.cz)
 * @author Vladyslav Kovalets (xkoval21@stud.fit.vutbr.cz)
 */

#include "string_t.h"
#include <stdlib.h>
#include <string.h>


#define INITIAL_CAPACITY 16
#define ALLOCATION_STEP  2


void str_dest(str_t *str)
{
    if (!str)
        return;
    free(str->ptr);
    str->ptr = NULL;
}


void str_clear(str_t* str)
{
    str->len = 0;
    memset(str->ptr, 0, str->cap);
}


bool str_const(str_t* str)
{   
    str->cap = INITIAL_CAPACITY;
    if (!(str->ptr = calloc(1, str->cap)))
        return false;
    str->len = 0;
    return true;
}

bool str_concat(str_t* dst, const char* src)
{
    int64_t srclen = strlen(src);
    while (dst->len + srclen > dst->cap - 1)
    {
        dst->cap *= ALLOCATION_STEP;
        
        if (!(dst->ptr = realloc(dst->ptr, dst->cap)))
            return false;
    }
    
    strcat(dst->ptr, src);
    dst->len += srclen;
    dst->ptr[dst->len] = '\0';

    return true;
}

void str_cpy(str_t* dst, str_t* src)
{
    dst->len = src->len;
    dst->cap = src->cap;
    strcpy(dst->ptr, src->ptr);
}


bool str_add_sign(str_t *str, char new_char)
{
    if ((str->len + 1) < str->cap)
    {
        str->ptr[str->len] = new_char;
        str->len++;
        str->ptr[str->len] = '\0';
    }
    else
    {
        str->cap *= ALLOCATION_STEP;
        
        if (!(str->ptr = realloc(str->ptr, str->cap)))
            return false;

        str->ptr[str->len] = new_char;
        str->len++;
    }
    return true;
}

bool str_cmp(str_t *first, const char *second)
{
    return !strcmp(first->ptr, second);
}

char str_last_sign(str_t* str)
{
    return str->ptr[str->len-1];
}
