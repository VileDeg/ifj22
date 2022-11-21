#ifndef __ERRORS__
#define __ERRORS__

#include <signal.h>

// #define TOKEN_OK 0
// #define RULE_OK 0
// #define REDUCE_OK 0
// #define EXPRESSION_OK 0
#define SUCCESS 0

#define ERROR_LEXICAL 1               //Error in lexical analyse (chybná struktura aktuálního lexému).       
#define ERROR_SYNTAX 2                //Error in syntax analyse.
#define ERROR_SEM_ID_DEF 3    //Error in semantic analyse (undefined function).
#define ERROR_SEM_TYPE_COMPAT 4     //Error in semantic analyse - wrong number/type of var./return values in calling function(or in returning from function).
#define ERROR_SEM_UNDEF_VAR 5    //Error in semantic analyse (undefined variable).
#define ERROR_SEM_RETURN 6 //!!
#define ERROR_SEM_EXPRESSION 7   //Error in semantic of t.k(!!) in arithmetic, string, relacnich(!!) expressions.
#define ERROR_SEM_OTHER 8        //Other semantic errors.
#define ERROR_INTERNAL 99             //Internal error(problems with allocation ...)

#define NUMFERRORS 9


#ifdef VILE_DEBUG

    #define VILE_DEBUGBRK raise(SIGTRAP)
    #define VILE_ASSERT(val, msg)\
        do{\
        if (!(val))\
        {\
            fprintf(stderr, "[%s]::ASSERT on line: %d, in function: %s\n\t%s\n",\
                __FILE__, __LINE__, __func__, msg);\
            VILE_DEBUGBRK;\
        }\
        } while(0)
#else
#define VILE_ASSERT(val, msg)
#endif //VILE_DEBUG

extern const char* g_ErrStr[NUMFERRORS];

// fprintf(stderr, "\033[1;31m");
// fprintf(stderr, "\033[0m");

#define _PRERRH(_errstr, _fmt, ...)\
    do{\
        fprintf(stderr, "[%s]: " _fmt "\n%s", _errstr, __VA_ARGS__);\
    } while(0)\
    
#define _PRERR(...) _PRERRH(__VA_ARGS__, "")

#define PRINT_ERROR(_errcode, ...) _PRERR(g_ErrStr[(_errcode)], __VA_ARGS__)
#define PRINT_ERROR_RET(_errcode, ...) do{\
        fprintf(stderr, "[%s]::COMPILER ERROR on line: %d, in function: %s\n",\
            __FILE__, __LINE__, __func__);\
        _PRERR(g_ErrStr[(_errcode)], __VA_ARGS__);\
        return _errcode;\
    } while(0)

#define INTERNAL_ERROR_RET(...) do{\
        _PRERR("ERROR_INTERNAL", __VA_ARGS__);\
        VILE_ASSERT(false, "");\
        return ERROR_INTERNAL;\
    } while(0)

// #define PRINT_ERROR_LEX(...) _PRERR("ERROR_LEXICAL", __VA_ARGS__)
// #define PRINT_ERROR_SYNT(...) _PRERR("ERROR_SYNTAX", __VA_ARGS__)
// #define PRINT_ERROR_SEM_UNDEF_FUNC(...) _PRERR("ERROR_SEMANTIC_UNDEFINED_FUNCTION", __VA_ARGS__)
// #define PRINT_ERROR_SEM_FUNC(...) _PRERR("ERROR_SEMANTIC_FUNCTION", __VA_ARGS__)
// #define PRINT_ERROR_SEM_UNDEF_VAR(...) _PRERR("ERROR_SEMANTIC_UNDEFINED_VARIABLE", __VA_ARGS__)
// #define PRINT_ERROR_SEM_RET(...) _PRERR("ERROR_SEMANTIC_RETURN", __VA_ARGS__)
// #define PRINT_ERROR_SEM_EXPR(...) _PRERR("ERROR_SEMANTIC_EXPRESSION", __VA_ARGS__)
// #define PRINT_ERROR_SEM_OTHER(...) _PRERR("ERROR_SEMANTIC_OTHER", __VA_ARGS__)
// #define PRINT_ERROR_INTERN(...) _PRERR("ERROR_INTERNAL", __VA_ARGS__)


#endif //__ERRORS__