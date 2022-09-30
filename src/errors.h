#ifndef __ERRORS__
#define __ERRORS__

#define TOKEN_OK 0

#define ERROR_LEXICAL 1               //Error in lexical analyse (chybná struktura aktuálního lexému).       
#define ERROR_SYNTAX 2                //Error in syntax analyse.
#define ERROR_SEMANTIC_UNDEFINED_FUNCTION 3    //Error in semantic analyse (undefined function).
#define ERROR_SEMANTIC_FUNCTION 4     //Error in semantic analyse - wrong number/type of var./return values in calling function(or in returning from function).
#define ERROR_SEMANTIC_UNDEFINED_VARIABLE 5    //Error in semantic analyse (undefined variable).
#define ERROR_SEMANTIC_RETURN 6 //!!
#define ERROR_SEMANTIC_EXPRESSION 7   //Error in semantic of t.k(!!) in arithmetic, string, relacnich(!!) expressions.
#define ERROR_SEMANTIC_OTHER 8        //Other semantic errors.
#define ERROR_INTERNAL 99             //Internal error(problems with allocation ...)

#endif //__ERRORS__