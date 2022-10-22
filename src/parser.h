#ifndef __PARSER_H__
#define __PARSER_H__

#include <symtable.h>
#include <scanner.h>

typedef struct 
{
    TSymtable globalTable;
	TSymtable localTable;
    Token token;
} ParserData;



int parser_parse();

#endif // __PARSER_H__