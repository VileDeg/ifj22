#include "string_t.h"
#include <stdlib.h>
#include <string.h>

//Allocation size.
#define INITIAL_CAPACITY 16
#define ALLOCATION_STEP  2

//Free memory that was allocated for string. 
void str_dest(str_t *str)
{
    free(str->ptr);
}

//Delete data from string.
void str_clear(str_t* str)
{
    str->len = 0;
    memset(str->ptr, 0, str->cap);
	//str->ptr[str->len] = '\0';
}

//Initialization of string.
bool str_const(str_t* str)
{   
    str->cap = INITIAL_CAPACITY;
    if (!(str->ptr = calloc(1, str->cap)))
        return false;
    str_clear(str);
    return true;
}

bool str_concat(str_t* dst, const char* src)
{
    size_t srclen = strlen(src);
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

//Add char to end of string.
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
