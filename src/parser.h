#ifndef __PARSER_H__
#define __PARSER_H__

#include "symtable.h"
#include "scanner.h"

#define GEN_CODE(_callback, ...)								\
	if (!_callback(__VA_ARGS__)) return ERROR_INTERNAL

typedef struct 
{
    TSymtable globalTable;
	TSymtable localTable;

    Token token;

    TData* current_func;
    TData* current_var;

    bool in_param_list;
    bool in_scope;
} ParserData;

int parse_file(FILE* fptr);

// Used only for testing in 'test_file' function (debug.h)
int parse_old();

#endif // __PARSER_H__