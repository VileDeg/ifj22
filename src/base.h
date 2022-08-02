#ifndef __BASE_H__
#define __BASE_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <errno.h>

typedef enum 
{
    MEMERR=-1
}errcode;

#define ERRPR(msg)\
    do{\
        fprintf(stderr, "[%s]::ERROR on line: %d, in function: %s\n\t%s\n",\
            __FILE__, __LINE__, __func__, msg);\
    } while(0)

#define MEMFAIL \
    do{\
        perror("Memory allocation failed!");\
        exit(MEMERR);\
    } while(0)

#define CALLOC(p, bytes) \
    do {\
        if (!(p = calloc(1, bytes)))\
        {\
            MEMFAIL;\
        }\
    } while(0)

#define FREE(p)\
    do {\
        free(p);\
        p = NULL;\
    } while(0)

#endif // __BASE_H__