#ifndef __STRING_T__
#define __STRING_T__

#include <stdbool.h>
#include <inttypes.h>
//#include "base.h"

//Struct of the allocation for string.
typedef struct
{
  	char*    ptr;                 // String. 
	uint64_t len;      // Length of string.
	uint64_t cap;  // Size of the allocated memory.
} str_t;

//Free memory that was allocated for the string.
void str_dest(str_t* str);

//str_t* str_set(const char* txt);

//Preparing to write data.
void str_clear(str_t* str);

//Initialization of string.
bool str_const(str_t* str);

//Add char to end of string.
bool str_add_sign(str_t* str, char new_char); 

//void str_copy(str_t* source, str_t *destination);

bool str_cmp(str_t* first, const char* second);

#endif //__STRING_T__