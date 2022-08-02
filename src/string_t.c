#include "string_t.h"

//Allocation size.
#define ALLOCATION_STEP 16

//Free memory that was allocated for string. 
void str_dest(str_t *str)
{
    free(str->ptr);
}

//Delete data from string.
void str_clear(str_t* str)
{
    if (!str) return;
    //Zeroes len.
    str->len = 0;	
    //Add end of line.
	str->ptr[str->len] = '\0';
}

//Initialization of string.
void str_const(str_t* str)
{   
    //Allocates memory.
    str->ptr = s_calloc(ALLOCATION_STEP);
    //Delete data from string.
    str_clear(str);
    //Updates allocation size information.
    str->cap = ALLOCATION_STEP;
}

str_t* str_set(const char* txt)
{
    str_t *tmp = s_calloc(sizeof(str_t));
    size_t len = strlen(txt);

    tmp->len = len;
    tmp->cap = len+1;
    tmp->ptr = s_calloc(tmp->cap);

    strncpy(tmp->ptr, txt, tmp->len);
    return tmp;
}

//Add char to end of string.
void str_add_sign(str_t *str, char new_char)
{
    //Checks if the allocated memory is enough.
    if ((str->len + 1) < (str->cap))
    {
        //Writes a new character to end of line.
        str->ptr[str->len] = new_char;
        //Increments length of the string.
        str->len++;
        //Add end of the line.
        str->ptr[str->len] = '\0';
    }
    else
    {
        //Creates a new size for allocation.
        int new_allocate_size = str->len + ALLOCATION_STEP;

        //Сhanges size of allocated memory block.
        str->ptr = s_realloc(str->ptr, new_allocate_size);
        
        //Updates allocation size information.
        str->cap = new_allocate_size;
        str_add_sign(str, new_char);
    }
}

void str_copy(str_t *source, str_t *destination)
{
    if(source->len >= destination->cap)
    {
        destination->cap = source->len + 1;
        destination->ptr = s_realloc(destination->ptr, destination->cap);
    }
    destination->len = source->len;
    strncpy(destination->ptr, source->ptr, destination->len);
}

bool str_cmp(str_t *first, const char *second)
{
    return !strcmp(first->ptr, second);
}
