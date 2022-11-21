#ifndef __DEBUG_H__
#define __DEBUG_H__


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "errors.h"
#include "scanner.h"

#ifdef IFJ22_DEBUG

    bool g_DebugOn;

    extern FILE* g_DebugOut;

    void set_debug_out(FILE* fptr);
    FILE* get_scan_out();
    FILE* get_pars_out();
    FILE* get_expr_out();

    #define DEBUGPR(...) do { VILE_ASSERT(g_DebugOut != NULL, "Output file not found");\
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
        FILE* scan_out, FILE* pars_out, FILE* expr_out, FILE* codegen_out);

    void debug_terminate();

    int test_stdin(FILE* scan_out);

    int test_scanner(FILE* source, bool show_source_contents, 
        FILE* scanner_output);

    void populate_rule_definitions();
    void print_rule_definitions();
    const char* get_rule_expansion_by_name(const char* rulename, int expnum);
#endif // IFJ22_DEBUG

#endif //__DEBUG_H__