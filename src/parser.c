#include "parser.h"
#include "errors.h"

#define GET_TOKEN()\
	if ((result = get_next_token(&data->token)) != TOKEN_OK)\
		return result

#define CHECK_TYPE(_type)\
	if (data->token.type != (_type)) return SYNTAX_ERR

#define CHECK_RULE(rule)\
	if ((result = rule(data))) return result

#define CHECK_KEYWORD(_keyword)\
	if (\
		data->token.type != TOKEN_TYPE_KEYWORD						\
		|| data->token.attribute.keyword != (_keyword)				\
	) return SYNTAX_ERR

#define GET_TOKEN_AND_CHECK_TYPE(_type)								\
	do {															\
		GET_TOKEN();												\
		CHECK_TYPE(_type);											\
	} while(0)

#define GET_TOKEN_AND_CHECK_RULE(rule)								\
	do {															\
		GET_TOKEN();												\
		CHECK_RULE(rule);											\
	} while(0)

#define GET_TOKEN_AND_CHECK_KEYWORD(_keyword)						\
	do {															\
		GET_TOKEN();												\
		CHECK_KEYWORD(_keyword);									\
	} while(0)


//<prolog> -> <?php declare ( strict_types = 1 ) ; <prog>
static int prolog(ParserData* pData)
{
    int result;

    return result;
}

static int prog(ParserData* pData)
{
    int result;

    //<prog> -> function ID ( <params> ) : <rettype> { <statement> } <prog> <end>
    if (pData->token.type == token_keyword && pData->token.value.keyword == keyword_function)
    {

    }
    //<prog> -> <statement> <end>

    return result;
}

static int init_data(ParserData* pd)
{
    
}

int parser_parse()
{
    ParserData pd;
    init_data(&pd);
    int result;
    if ((result = scanner_get_next_token(&pd.token)) == TOKEN_OK)
    {
        result = prolog(&pd);
    }

    return result;
}