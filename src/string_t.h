/**
 * Project: Implementation of imperative language IFJ22 compiler.
 * 
 * @author Vadim Goncearenco  (xgonce00@stud.fit.vutbr.cz)
 * @author Vladyslav Kovalets (xkoval21@stud.fit.vutbr.cz)
 */

#ifndef __STRING_T__
#define __STRING_T__

#include <stdbool.h>
#include <inttypes.h>

/**
 * Dynamic string struct.
 */
typedef struct
{
  	char*    ptr;                 // String. 
	int64_t len;      // Length of string.
	int64_t cap;  // Size of the allocated memory.
} str_t;

/**
 * Destructs dynamic string.
 * @param str string
 */
void str_dest(str_t* str);

/**
 * Concatenates dynamic string with C-string.
 * @param dst destination
 * @param src source 'C-string'
 * @return True if success.
 */
bool str_concat(str_t* dst, const char* src);

/**
 * Clears contents of dynamic string.
 * @param str string
 */
void str_clear(str_t* str);

/**
 * Constructs dynamic string.
 * @param str string
 * @return True if success.
 */
bool str_const(str_t* str);

/**
 * Appends character to dynamic string.
 * @param str string
 * @param sign sign
 * @return True if success.
 */
bool str_add_sign(str_t* str, char sign); 

/**
 * Compares dynamic string to C-string
 * @param dst destination
 * @param src source 'C-string'
 * @return True if success.
 */
bool str_cmp(str_t* dst, const char* src);

/**
 * Creates deep copy of dynamic string.
 * @param dst destination
 * @param src source
 */
void str_cpy(str_t* dst, str_t* src);

/**
 * Returns last sign of dynamic string.
 * @param str string
 * @return Last sign.
 */
char str_last_sign(str_t* str);

#endif //__STRING_T__