#ifndef __PARSER_H__
#define __PARSER_H__

#include <symtable.h>
#include <scanner.h>

#define GEN_CODE(_callback, ...)								\
	if (!_callback(__VA_ARGS__)) return ERROR_INTERNAL

typedef struct 
{
    TSymtable globalTable;
	TSymtable localTable;
    Token token;
} ParserData;



int parser_parse();

#endif // __PARSER_H__