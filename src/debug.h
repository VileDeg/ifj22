#ifndef __DEBUG_H__
#define __DEBUG_H__

#include "base.h"
#include "scanner.h"
#include "stack_t.h"

void lexical_test(const char* filename);

void stack_print_int(stack_t* st);

#endif //__DEBUG_H__