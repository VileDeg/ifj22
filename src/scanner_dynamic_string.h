
#ifndef SCANNER_DYNAMIC_STRING
#define SCANNER_DYNAMIC_STRING

#include <stdbool.h>


//Struct of the allocation for string.
typedef struct dyn_str
{
  char *string;                 // String. 
	unsigned int str_length;      // Length of string.
	unsigned int allocated_size;  // Size of the allocated memory.
} Allocation;

//Free memory that was allocated for the string.
void string_free(Allocation *String);

Allocation * string_set(char * txt);

//Preparing to write data.
void string_clear(Allocation *String);

//Initialization of string.
bool string_init(Allocation *String);

//Add char to end of string.
bool string_add_sign(Allocation *String, char new_char); 

bool string_copy(Allocation *source, Allocation *destination);

bool string_cmp(Allocation *first, char *second);

#endif