#ifndef __PARSER_H__
#define __PARSER_H__

#include "symtable.h"
#include "scanner.h"
#include "stack_t.h"

#define GEN_CODE(_callback, ...)								\
	if (!_callback(__VA_ARGS__)) return ERROR_INTERNAL

typedef struct 
{
    TSymtable globalTable;
	TSymtable localTable;

    Token token;

    TData* rhs_func;
    TData* lhs_var;

    bool in_param_list;
    bool last_rule_was_eps;
    bool in_local_scope;
    
    int  param_index;
} ParserData;

int parse_file(FILE* fptr);

#endif // __PARSER_H__