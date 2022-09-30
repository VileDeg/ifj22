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
	str->ptr[str->len] = '\0';
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

// str_t* str_set(const char* txt)
// {
//     str_t *tmp = s_calloc(sizeof(str_t));
//     size_t len = strlen(txt);

//     tmp->len = len;
//     tmp->cap = len+1;
//     tmp->ptr = s_calloc(tmp->cap);

//     strncpy(tmp->ptr, txt, tmp->len);
//     return tmp;
// }

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

// void str_copy(str_t *source, str_t *destination)
// {
//     if(source->len >= destination->cap)
//     {
//         destination->cap = source->len + 1;
//         destination->ptr = s_realloc(destination->ptr, destination->cap);
//     }
//     destination->len = source->len;
//     strncpy(destination->ptr, source->ptr, destination->len);
// }

bool str_cmp(str_t *first, const char *second)
{
    return !strcmp(first->ptr, second);
}
