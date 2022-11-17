#ifndef __PARSER_H__
#define __PARSER_H__

#include "symtable.h"
#include "scanner.h"
#include "stack_t.h"
#include "debug.h"

typedef struct
{
    TSymtable globalTable;
	TSymtable localTable;

    Token token;

    TData* rhs_func;
    TData* lhs_var;

    str_t    var_name;
    DataType var_type;

    bool in_param_list;
    bool last_rule_was_eps;
    bool in_local_scope;
    bool func_questionmark;
    bool rvalue_assign;
    
    int  param_index;
    int  label_index;
    int  label_deep;
} ParserData;

int parse_file(FILE* fptr);

#endif // __PARSER_H__