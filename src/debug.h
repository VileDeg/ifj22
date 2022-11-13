#ifndef __DEBUG_H__
#define __DEBUG_H__


#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <stdbool.h>

#include "scanner.h"

#ifdef IFJ22_DEBUG

    #define IFJ22_DEBUGBRK raise(SIGTRAP)
    #define IFJ22_ASSERT(val, msg)\
        do{\
        if (!(val))\
        {\
            fprintf(stderr, "[%s]::ASSERT on line: %d, in function: %s\n\t%s\n",\
                __FILE__, __LINE__, __func__, msg);\
            IFJ22_DEBUGBRK;\
        }\
        } while(0)


    #define ERRPR(msg)\
        do{\
            fprintf(stderr, "[%s]::ERROR on line: %d, in function: %s\n\t%s\n",\
                __FILE__, __LINE__, __func__, msg);\
        } while(0)

    #define WRNPR(msg)\
        do{\
            fprintf(stdout, "[%s]::WARNING on line: %d, in function: %s\n\t%s\n",\
                __FILE__, __LINE__, __func__, msg);\
        } while(0)

    extern FILE* g_DebugOut;

    void set_debug_out(FILE* fptr);
    FILE* get_scan_out();
    FILE* get_pars_out();
    FILE* get_expr_out();

    #define DEBUGPR(...) do { IFJ22_ASSERT(g_DebugOut != NULL, "Output file not found");\
        fprintf(g_DebugOut, __VA_ARGS__); fflush(g_DebugOut); } while(0)
    
    #define EXPRDBGPR(...) do {\
        set_debug_out(get_expr_out());\
        DEBUGPR(__VA_ARGS__);\
    } while(0)

    typedef struct{
        int scan;
        int pars;
    } test_retcodes;

    void debug_print_token(Token tk);

    void debug_setup(FILE* source, bool show_source_contents, 
        FILE* scan_out, FILE* pars_out, FILE* expr_out);

    void debug_terminate();

    int test_stdin(FILE* scan_out);

    int test_scanner(FILE* source, bool show_source_contents, 
        FILE* scanner_output);

    void populate_rule_definitions();
    void print_rule_definitions();
    const char* get_rule_expansion_by_name(const char* rulename, int expnum);

#else
    #define IFJ22_ASSERT(val, msg)
#endif // IFJ22_DEBUG

#endif //__DEBUG_H__