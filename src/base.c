#include "base.h"

void* s_calloc(size_t size)
{
    void* p = calloc(1, size);
    if (p == NULL)
    {
        perror("Memory allocation failed!");
        exit(MEMERR);
    }
    return p;
}

void* s_realloc(void* ptr, size_t size)
{
    void* tmp = realloc(ptr, size);
    if (tmp == NULL)
    {
        perror("Memory reallocation failed!");
        exit(MEMERR);
    }
    return tmp;
}
