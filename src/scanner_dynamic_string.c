
#include <stdlib.h>
#include <string.h>

#include "scanner_dynamic_string.h"

//Allocation size.
#define dynamic_str_length 10

//Free memory that was allocated for string. 
void string_free(Allocation *String)
{
    free(String->string);
}

//Delete data from string.
void string_clear(Allocation *String)
{
    if (!String) return;
    //Zeroes str_length.
    String->str_length = 0;	
    
    //Add end of line.
	String->string[String->str_length] = '\0';
}

//Initialization of string.
bool string_init(Allocation *String)
{   
    //Allocates memory.
    String->string = malloc(dynamic_str_length*sizeof(char));
    
    //Сheck if allocation was successful.
    if(String->string == NULL)
    {
        string_free(String);
        return false;
    }
    else
    {
        //Delete data from string.
        string_clear(String);

        //Updates allocation size information.
        String->allocated_size = dynamic_str_length;
        return true;
    }
}
Allocation * string_set(char * txt){
    Allocation *tmp = malloc(sizeof(struct dyn_str));
    unsigned int str_length = strlen(txt);
    tmp->str_length = str_length;
    tmp->allocated_size = str_length;
    tmp->string = calloc(str_length, sizeof(char));
    strcpy(tmp->string,txt);
    return tmp;
}

//Add char to end of string.
bool string_add_sign(Allocation *String, char new_char)
{
    //Checks if the allocated memory is enough.
    if ((String->str_length + 1) < (String->allocated_size))
    {
        //Writes a new character to end of line.
        String->string[String->str_length] = new_char;
        //Increments length of the string.
        String->str_length++;
        //Add end of the line.
        String->string[String->str_length] = '\0';
    }
    else
    {
        //Creates a new size for allocation.
        int new_allocate_size = String->str_length + dynamic_str_length;

        //Сhanges size of allocated memory block.
        if ((String->string = (char*) realloc(String->string, new_allocate_size)) == NULL)
            return false;
        
        //Updates allocation size information.
        String->allocated_size = new_allocate_size;
        string_add_sign(String, new_char);
    }
    return true;
}

bool string_copy(Allocation *source, Allocation *destination)
{
    if(source->str_length >= destination->allocated_size)
    {
        if(!(destination->string = (char *) realloc(destination->string, source->str_length + 1)))
            return false;
        destination->allocated_size = source->str_length + 1;
    }
    strcpy(destination->string, source->string);
    destination->str_length = source->str_length;
    return true;
}

bool string_cmp(Allocation *first, char *second)
{
    if (strcmp(first->string, second) == 0) return true;
	return false;
}
