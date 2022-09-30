#ifndef __PARSER_H__
#define __PARSER_H__
#include "scanner.h"

typedef struct 
{
    Token token;
} ParserData;


int parser_parse();

#endif // __PARSER_H__