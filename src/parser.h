#ifndef __PARSER_H__
#define __PARSER_H__

#include "symtable.h"
#include "scanner.h"
//#include "stack_t.h"
#include "debug.h"

typedef struct
{
    TSymtable globalTable;
	TSymtable localTable;

    Token token;

    TData* rhs_func;
    TData* lhs_var;

    // str_t    var_name;
    // DataType var_type;

    bool in_param_list;
    bool block_next_token;
    bool in_local_scope;
    bool func_questionmark;
    bool var_not_yet_def;
    bool get_next_tk_from_stack;
    
    int  param_index;
    int  label_index;
    int  label_deep;
} ParserData;

int parse_file(FILE* fptr);

int _get_next_token(ParserData* pd);

#endif // __PARSER_H__