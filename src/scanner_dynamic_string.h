
#ifndef SCANNER_DYNAMIC_STRING
#define SCANNER_DYNAMIC_STRING

#include "base.h"

//Struct of the allocation for string.
typedef struct
{
  	char*    ptr;                 // String. 
	unsigned len;      // Length of string.
	unsigned cap;  // Size of the allocated memory.
} str_t;

//Free memory that was allocated for the string.
void str_dest(str_t* str);

str_t* str_set(const char* txt);

//Preparing to write data.
void str_clear(str_t* str);

//Initialization of string.
void str_const(str_t* str);

//Add char to end of string.
void str_add_sign(str_t* str, char new_char); 

void str_copy(str_t* source, str_t *destination);

bool str_cmp(str_t* first, const char* second);

#endif