#ifndef __PARSER_H__
#define __PARSER_H__

#include "symtable.h"
#include "scanner.h"
#include "vector_t.h"
#include "debug.h"

GENERATE_VECTOR_DECLARATION(Token, tk);

/**
 * Data for source code parsing.
 */
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
    
    int mode;

    bool in_param_list;
    bool in_local_scope;
    bool in_if_while;
    bool block_next_token;
    bool return_found;
    bool var_not_yet_def;
    
    int64_t param_index;
    int64_t label_index;
    int64_t label_deep;
} ParserData;

/**
 * Entry point for code parsing.
 * @param fptr file pointer
 * @return Error code.
 */
int64_t parse_file(FILE* fptr);

/**
 * Encapsulates scanner function to get next token.
 * @param pd parser data
 * @return Error code.
 */
int64_t parser_get_token(ParserData* pd);

#endif // __PARSER_H__