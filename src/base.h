#ifndef __BASE_H__
#define __BASE_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>




typedef enum 
{
    MEMERR=-1111
} errcode;

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


static void print_int(int x)     { printf("int: %d\n", x); }
static void print_float(float x) { printf("float: %g\n", x); }
static void print_default()      { ERRPR("Unsupported type."); }

#define GENERIC_DATA_PRINT(data) _Generic((data), \
        int:     print_int, \
        float:   print_float, \
        default: print_default)(data)

void* s_calloc(size_t size);
void* s_realloc(void* ptr, size_t size);

#define S_FREE(p)\
    do {\
        free(p);\
        p = NULL;\
    } while(0)

#endif // __BASE_H__