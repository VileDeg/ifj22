#ifndef __ERRORS__
#define __ERRORS__

#define TOKEN_OK 0
#define RULE_OK 0

#define ERROR_LEXICAL 1               //Error in lexical analyse (chybná struktura aktuálního lexému).       
#define ERROR_SYNTAX 2                //Error in syntax analyse.
#define ERROR_SEMANTIC_UNDEFINED_FUNCTION 3    //Error in semantic analyse (undefined function).
#define ERROR_SEMANTIC_FUNCTION 4     //Error in semantic analyse - wrong number/type of var./return values in calling function(or in returning from function).
#define ERROR_SEMANTIC_UNDEFINED_VARIABLE 5    //Error in semantic analyse (undefined variable).
#define ERROR_SEMANTIC_RETURN 6 //!!
#define ERROR_SEMANTIC_EXPRESSION 7   //Error in semantic of t.k(!!) in arithmetic, string, relacnich(!!) expressions.
#define ERROR_SEMANTIC_OTHER 8        //Other semantic errors.
#define ERROR_INTERNAL 99             //Internal error(problems with allocation ...)

#define _PRERRH(err, fmt, ...)\
    do{\
        fprintf(stderr, "\033[1;31m");\
        fprintf(stderr, "[" err "]: " fmt "\n%s", __VA_ARGS__);\
        fprintf(stderr, "\033[0m");\
    } while(0)\
    
#define _PRERR(...) _PRERRH(__VA_ARGS__, "")
#define ERROR_LEX(...) _PRERR("ERROR_LEXICAL", __VA_ARGS__)
#define ERROR_SYNT(...) _PRERR("ERROR_SYNTAX", __VA_ARGS__)
#define ERROR_SEM_UNDEF_FUNC(...) _PRERR("ERROR_SEMANTIC_UNDEFINED_FUNCTION", __VA_ARGS__)
#define ERROR_SEM_FUNC(...) _PRERR("ERROR_SEMANTIC_FUNCTION", __VA_ARGS__)
#define ERROR_SEM_UNDEF_VAR(...) _PRERR("ERROR_SEMANTIC_UNDEFINED_VARIABLE", __VA_ARGS__)
#define ERROR_SEM_RET(...) _PRERR("ERROR_SEMANTIC_RETURN", __VA_ARGS__)
#define ERROR_SEM_EXPR(...) _PRERR("ERROR_SEMANTIC_EXPRESSION", __VA_ARGS__)
#define ERROR_SEM_OTHER(...) _PRERR("ERROR_SEMANTIC_OTHER", __VA_ARGS__)
#define ERROR_INTERN(...) _PRERR("ERROR_INTERNAL", __VA_ARGS__)

#endif //__ERRORS__