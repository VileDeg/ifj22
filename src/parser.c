#include "parser.h"
#include "errors.h"

#include "debug.h"

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



#define CHECK_KEYWORD(kw) pd->token.type == token_keyword && pd->token.value.keyword == keyword_##kw
#define NEXT_TOKEN()\
	if ((result = scanner_get_next_token(&pd->token)) != TOKEN_OK)\
		return result
#define CHECK_TYPE(type_postfix)\
	if (pd->token.type != token_##type_postfix) return ERROR_SYNTAX
#define CHECK_NEXT_TOKEN(type)								\
	do {															\
		NEXT_TOKEN();												\
		CHECK_TYPE(type);											\
	} while(0)
#define RUN_NEXT_RULE(_rule)\
	if ((result = _rule(pd))) return result


#define DEBUG_PRINT_RULE(expnum)\
    do{\
        printf("%s -> %s\n", __func__, get_rule_expansion_by_name(__func__, (expnum)));\
    }while(0)

static int init_data(ParserData* pd)
{
    
}

static bool check_source_code_start()
{
	FILE* fptr = scanner_get_file();
	uint64_t maxlen = 5;
    const char* line = "<?php";
	//First token was already read so we need to ungetc TODO:?
	ungetc('<', fptr);
	for (uint64_t i = 0; i < maxlen; i++)
	{
		int c;
		if ((c=fgetc(fptr)) != line[i])
			return false;
	}

	return true;	
}


static int prog(ParserData* pd)
{
    // int result;

    // //<prog> -> function ID ( <params> ) : <rettype> { <statement> } <prog> <end>
    // if (CHECK_KEYWORD(function))
    // {
	// 	CHECK_NEXT_TOKEN(ID);
	// 	CHECK_NEXT_TOKEN(left_bracket);

	// 	RUN_NEXT_RULE(params);
		
	// 	CHECK_NEXT_TOKEN(right_bracket);
	// 	CHECK_NEXT_TOKEN(colon);

	// 	RUN_NEXT_RULE(rettype);

	// 	CHECK_NEXT_TOKEN(left_curly_bracket);

	// 	RUN_NEXT_RULE(statement);
		
	// 	CHECK_NEXT_TOKEN(right_curly_bracket);

	// 	RUN_NEXT_RULE(prog);

	// 	RUN_NEXT_RULE(end);
    // }
	// //<prog> -> <statement> <end>
	// else
	// {
	// 	RUN_NEXT_RULE(statement);

	// 	RUN_NEXT_RULE(end);
	// }

    // return result;
}

//<prolog> -> <?php declare ( strict_types = 1 ) ; <prog>
static int prolog(ParserData* pd)
{
	DEBUG_PRINT_RULE(0);

    int result;
	int errtype = ERROR_SYNTAX;
	if (!check_source_code_start())
		return errtype;
	
	scanner_get_next_token(&pd->token);
	


	// CHECK_NEXT_TOKEN(ID);
	// if (!str_cmp(pd->token.value.String, "declare"))
	// 	return errtype;
	
	// CHECK_NEXT_TOKEN(left_bracket);
	// CHECK_NEXT_TOKEN(ID);
	// if (!str_cmp(pd->token.value.String, "strict_types"))
	// 	return errtype;
		
	// CHECK_NEXT_TOKEN(equal_sign);
	// CHECK_NEXT_TOKEN(integer);
	// if (pd->token.value.integer != 1)
	// 	return errtype;

	// CHECK_NEXT_TOKEN(right_bracket);
	// CHECK_NEXT_TOKEN(semicolon);

	//RUN_NEXT_RULE(prog);

    return result;
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