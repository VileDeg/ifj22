#ifndef __DEBUG_H__
#define __DEBUG_H__


#include <stdio.h>

#include <assert.h>
#include <stdbool.h>



#define ASSERT(val, msg)\
    do{\
    if (!val)\
    {\
        fprintf(stderr, "[%s]::ASSERT on line: %d, in function: %s\n\t%s\n",\
            __FILE__, __LINE__, __func__, msg);\
        assert(val);\
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


void lexical_test(const char* filename, bool show_contents);



#endif //__DEBUG_H__