#ifndef __PARSER_H__
#define __PARSER_H__

typedef struct                                                                  //FIX ME unknown type name TSymtable
{
    TSymtable globalTable;
	TSymtable localTable;
    Token token;
} ParserData;

int parser_parse();

#endif // __PARSER_H__