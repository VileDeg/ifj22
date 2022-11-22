#ifndef __PARSER_H__
#define __PARSER_H__

#include "symtable.h"
#include "scanner.h"
#include "vector_t.h"
#include "debug.h"

GENERATE_VECTOR_DECLARATION(Token, tk);

typedef struct
{
    TSymtable globalTable;
	TSymtable localTable;

    Token token;

    TData* rhs_func;
    TData* lhs_var;
    TData* current_func;

    tkvec_t tk_vec;
    tkelem_t* front_ptr;

    bool in_param_list;
    bool block_next_token;
    bool in_local_scope;
    bool return_found;
    bool var_not_yet_def;
    int mode;

    bool in_if_while;
    
    int  param_index;
    int  label_index;
    int  label_deep;
} ParserData;

int parse_file(FILE* fptr);

int _get_next_token(ParserData* pd);

#endif // __PARSER_H__