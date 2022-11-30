#ifndef __ERRORS_H__
#define __ERRORS_H__

#define SUCCESS 0

#define ERROR_LEXICAL         1     //Error in lexical analysis (bad actual lexical unit).       
#define ERROR_SYNTAX          2     //Error in syntax analysis (bad program syntax, missing header, etc.).
#define ERROR_SEM_ID_DEF      3     //Error in semantic analysis (undefined function, function redefenition).
#define ERROR_SEM_TYPE_COMPAT 4     //Error in semantic analysis (wrong number/type of argument in function call, wrong function return type).
#define ERROR_SEM_UNDEF_VAR   5     //Error in semantic analysis (undefined variable).
#define ERROR_SEM_RETURN      6     //Error in semantic analysis (missing or redundant expression in return statement).
#define ERROR_SEM_EXPRESSION  7     //Error in semantic analysis (operand type incompatible).
#define ERROR_SEM_OTHER       8     //Error in semantic analysis (other errors).

#define ERROR_INTERNAL        99    //Internal error (allocation fail or other).

#ifdef VILE_DEBUG //Macro for compiling in debug mode.
    #include <signal.h>

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

//Helper macro for 'error print' macro.
#define _PRERR(_errstr, _fmt, ...)\
    do{\
        fprintf(stderr, "[%s]: " _fmt "\n%s", _errstr, __VA_ARGS__);\
    } while(0)\

//Prints error information and returns error code.
#define PRINT_ERROR_RET(_errcode, ...) do{\
        fprintf(stderr, "[%s]:: COMPILE-TIME ERROR on line: %d, in function: %s\n",\
            __FILE__, __LINE__, __func__);\
        _PRERR(#_errcode, __VA_ARGS__, "");\
        return _errcode;\
    } while(0)

//Helper macro for 'error print' in case of internal error.
#define INTERNAL_ERROR_RET(...) do{\
        VILE_ASSERT(false, "internal error.");\
        PRINT_ERROR_RET(ERROR_INTERNAL, __VA_ARGS__);\
    } while(0)

#endif //__ERRORS_H__