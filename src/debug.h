#ifndef __DEBUG_H__
#define __DEBUG_H__


#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <stdbool.h>


#define DEBUGBRK raise(SIGTRAP)
#define ASSERT(val, msg)\
    do{\
    if (!(val))\
    {\
        fprintf(stderr, "[%s]::ASSERT on line: %d, in function: %s\n\t%s\n",\
            __FILE__, __LINE__, __func__, msg);\
        DEBUGBRK;\
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
#define SET_DEBUG_OUT(fptr) g_DebugOut = (fptr)
#define DEBUGPR(...) do{ ASSERT(g_DebugOut != NULL, ""); fprintf(g_DebugOut, __VA_ARGS__); }while(0)

void test_file(FILE* source, bool show_source_contents, 
    FILE* scanner_output, FILE* parser_output);

void populate_rule_definitions();
void print_rule_definitions();
const char* get_rule_expansion_by_name(const char* rulename, int expnum);

#endif //__DEBUG_H__