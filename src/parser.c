#include <string.h>

#include "parser.h"
#include "errors.h"
#include "debug.h"
#include "scanner.h"
#include "symtable.h"

#define _DPRNR(_n)\
    do{\
        DEBUGPR("%s -> %s\n", __func__, get_rule_expansion_by_name(__func__, (_n)));\
    }while(0)

//#define DEF_RLNM int _RLNM = -1
//#define _DPRNR(_n) DEBUG_PRINT_RULE(_n)

#define RES result
#define DEF_RES int RES
#define RULE_OPEN DEF_RES


#define GET_NEXT_TOKEN()\
	if ((RES = scanner_get_next_token(&pd->token)) != TOKEN_OK)\
		return RES

#define TYPE_IS(type_postfix) (pd->token.type == token_##type_postfix)

#define KEYWORD_IS(kw) (pd->token.type == token_keyword && pd->token.value.keyword == keyword_##kw)

//Never use with more than 1 argument!
#define CHECK_TYPE(_type_postfix, ...)\
	if (!TYPE_IS(_type_postfix##__VA_ARGS__)) return ERROR_SYNTAX
	
#define CHECK_KEYWORD(_kw)\
	if (KEYWORD_IS(_kw)) return ERROR_SYNTAX

#define GET_N_CHECK_TYPE(_type)\
	do {\
		GET_NEXT_TOKEN();\
		CHECK_TYPE(_type);\
	} while(0)

#define GET_N_CHECK_KEYWORD(_kw)\
	do {\
		GET_NEXT_TOKEN();\
		CHECK_KEYWORD(_kw);\
	} while(0)

#define RULE_GOOD(_rule) (_rule(pd) == RULE_OK)

#define CHECK_RULE(_rule)\
	if ((RES = _rule(pd))) return RES

#define GET_N_CHECK_RULE(_rule)\
	do {\
		GET_NEXT_TOKEN();\
		CHECK_RULE(_rule);\
	} while(0)

#define GET_N_RUN_RULE(_rule)\
	do {\
		GET_NEXT_TOKEN();\
		CHECK_RULE(_rule);\
	} while(0)

#define HAS_DOLLAR (pd->token.value.String->ptr[0] == '$')

#define IS_VAR_ID (TYPE_IS(ID) && HAS_DOLLAR)

#define CHECK_DOLLAR if (!HAS_DOLLAR) return ERROR_SYNTAX

#define GET_N_CHECK_VAR_ID GET_N_CHECK_TYPE(ID); CHECK_DOLLAR

// typedef struct 
// {
//     TSymtable globalTable;
// 	TSymtable localTable;
//     Token token;
// } ParserData;

static int init_data(ParserData* pd)
{
    symtable_init(&pd->globalTable);
	symtable_init(&pd->localTable);
}

static bool is_void_type(ParserData* pd)
{
	const char* str = pd->token.value.String->ptr;
	return pd->token.type == token_keyword && !strcmp(str, "void");
}

static int type(ParserData* pd);
static int value(ParserData* pd);
static int def_var(ParserData* pd);
static int rhs_value(ParserData* pd);
static int statement(ParserData* pd);
static int expression(ParserData* pd);
static int rettype(ParserData* pd);
static int arg(ParserData* pd);
static int arg_n(ParserData* pd);
static int params(ParserData* pd);
static int param_n(ParserData* pd);
static int prog(ParserData* pd);
static int prolog(ParserData* pd);
static int scoped_stat(ParserData* pd);
static int compound_stat(ParserData* pd);
static int end(ParserData* pd);

static int expression(ParserData* pd)
{
	printf("<%s> -> ???\n", __func__);
}

static int value(ParserData* pd)
{
	RULE_OPEN;

	//<value> -> INT_VALUE
	if (TYPE_IS(integer))
	{
		_DPRNR(0);
	}
	//<value> -> FLOAT_VALUE
	else if (TYPE_IS(float))
	{
		_DPRNR(1);
	}
	//<value> -> STRING_VALUE
	else if (TYPE_IS(string))
	{
		_DPRNR(2);
	}
	//<value> -> $ID
	else if (IS_VAR_ID)
	{
		_DPRNR(3);
	}
	else
		return ERROR_SYNTAX;

	return RULE_OK;
}

static int type(ParserData* pd)
{
	//<type> -> float
	//<type> -> int
	//<type> -> string
	//<type> -> ? float
	//<type> -> ? int
	//<type> -> ? string
	RULE_OPEN;
	CHECK_TYPE(keyword);
	
	int _rulenr = 0;
	const char* str = pd->token.value.String->ptr;
	if (str[0] == '?')
	{
		//Type can be null!
		++str;
		_rulenr += 3;
	}
	if (!strcmp(str, "float"))
	{
		_DPRNR(_rulenr+0);
	}
	else if (!strcmp(str, "int"))
	{
		_DPRNR(_rulenr+1);
	}
	else if (!strcmp(str, "string"))
	{
		_DPRNR(_rulenr+2);
	}
	else
		return ERROR_SYNTAX;

	return RULE_OK;
}

static int end(ParserData* pd)
{
	RULE_OPEN;

	//<end> -> ?> EOF
	GET_NEXT_TOKEN();
	if (TYPE_IS(end))
	{
		_DPRNR(0);
		
	}
	//<end> -> EOF
	else
	{
		_DPRNR(1);
		CHECK_TYPE(EOF);
	}

	return RULE_OK;
}

static int rhs_value(ParserData* pd)
{
	RULE_OPEN;

	//GET_N_CHECK_TYPE...
	DEBUGPR("%s -> ???\n", __func__);
	//Will be looked up in symtable
	//<rhs_value> -> ID ( <args> )
	//<rhs_value> -> <expression>
	//<rhs_value> -> reads ( <args> )
	//<rhs_value> -> readi ( <args> )
	//<rhs_value> -> readf ( <args> )
	//<rhs_value> -> write ( <args> )
	//<rhs_value> -> floatval ( <args> )
	//<rhs_value> -> intval ( <args> )
	//<rhs_value> -> strval ( <args> )
	//<rhs_value> -> strlen ( <args> )
	//<rhs_value> -> substring ( <args> )
	//<rhs_value> -> ord ( <args> )
	//<rhs_value> -> chr ( <args> )

	return RULE_OK;
}

static int def_var(ParserData* pd)
{
	RULE_OPEN;

	//<def_var> -> = <rhs_value>
	if (TYPE_IS(equal_sign))
	{
		_DPRNR(0);
		CHECK_RULE(rhs_value);
	}
	//<def_var> -> eps
	else
		_DPRNR(1);


	return RULE_OK;
}

static int scoped_stat(ParserData* pd)
{
	RULE_OPEN;

	//<scoped_stat> -> { <statement> }
	_DPRNR(0);
	GET_N_CHECK_TYPE(left_curly_bracket);

	GET_N_CHECK_RULE(statement);

	GET_N_CHECK_TYPE(right_curly_bracket);

	return RULE_OK;
}

static int compound_stat(ParserData* pd)
{
	RULE_OPEN;

	//<compound_stat> -> ( <expression> ) <scoped_stat>
	_DPRNR(0);
	GET_N_CHECK_TYPE(left_bracket);

	CHECK_RULE(expression);

	GET_N_CHECK_TYPE(right_bracket);

	CHECK_RULE(scoped_stat);

	return RULE_OK;
}

static int statement(ParserData* pd)
{
	RULE_OPEN;

	//<statement> -> $ID <def_var>; <statement>
	if (IS_VAR_ID)
	{
		_DPRNR(0);
		GET_N_CHECK_RULE(def_var);

		GET_N_CHECK_TYPE(semicolon);

		GET_N_CHECK_RULE(statement);
	}
	//<statement> -> if <compound_stat> else <scoped_stat> <statement>
	else if (KEYWORD_IS(if))
	{
		_DPRNR(1);
		CHECK_RULE(compound_stat);

		GET_N_CHECK_KEYWORD(else);

		CHECK_RULE(scoped_stat);

		GET_N_CHECK_RULE(statement);
	}
	//<statement> -> while <compound_stat> <statement>
	else if (KEYWORD_IS(while))
	{
		_DPRNR(2);
		CHECK_RULE(compound_stat);

		GET_N_CHECK_RULE(statement);
	}
	//<statement> -> return <expression>
	else if (KEYWORD_IS(return))
	{
		_DPRNR(3);
		CHECK_RULE(expression);
	}
	//<statement> -> eps
	
	return RULE_OK;
}

static int rettype(ParserData* pd)
{
	RULE_OPEN;

	//<rettype> -> void
	GET_NEXT_TOKEN();
	if (is_void_type(pd))
	{
		_DPRNR(0);
	}
	else
	{
		//<rettype> -> <type>
		_DPRNR(1);
		CHECK_RULE(type);
	}

	return RULE_OK;
}

static int arg(ParserData* pd)
{
	RULE_OPEN;

	//<arg> -> <value> <arg_n>
	if (RULE_GOOD(value))
	{
		_DPRNR(0);
		GET_N_CHECK_RULE(arg_n);
	}
	else
		_DPRNR(1);

	//<arg> -> eps

	return RULE_OK;
}

static int arg_n(ParserData* pd)
{
	RULE_OPEN;

	//<arg_n> -> , <value> <arg_n>
	if (TYPE_IS(comma))
	{
		_DPRNR(0);
		GET_N_CHECK_RULE(value);
		GET_N_CHECK_RULE(arg_n);
	}
	else
		_DPRNR(1);
	//<arg_n> -> eps
	
	return RULE_OK;
}

static int param_n(ParserData* pd)
{
	RULE_OPEN;
	//<param_n> -> , <type> $ ID <param_n>
	//GET_N_CHECK_TYPE(comma);
	
	if (TYPE_IS(comma))
	{
		_DPRNR(0);
		CHECK_RULE(type);

		GET_N_CHECK_VAR_ID;

		GET_N_CHECK_RULE(param_n);
	}
	else
		_DPRNR(0);
	//<param_n> -> eps

	return RULE_OK;
}

static int params(ParserData* pd)
{
	RULE_OPEN;

	//<params> -> <type> $ ID <param_n>
	
	if (RULE_GOOD(type))
	{
		_DPRNR(0);
		GET_N_CHECK_VAR_ID;

		GET_N_CHECK_RULE(param_n);
	}
	else
		_DPRNR(1);
	//<params> -> eps
	
	return RULE_OK;
}

static int prog(ParserData* pd)
{
	RULE_OPEN;

    //<prog> -> function ID ( <params> ) : <rettype> <scoped_stat> <prog>
	GET_NEXT_TOKEN();
    if (KEYWORD_IS(function))
    {
		_DPRNR(0);
		GET_N_CHECK_TYPE(ID);
		// {
		// 	bool internal_error = false;
		// 	symtable_add_symbol(&pd->globalTable, pd->token.value.String->ptr, &internal_error);
		// 	if (internal_error)
		// 	{
		// 		return ERROR_INTERNAL;
		// 	}
		// }		
		GET_N_CHECK_TYPE(left_bracket);

		GET_N_CHECK_RULE(params);
		
		GET_N_CHECK_TYPE(right_bracket);
		GET_N_CHECK_TYPE(colon);

		CHECK_RULE(rettype);

		GET_N_CHECK_RULE(scoped_stat);
		
		CHECK_RULE(prog);
    }
	//<prog> -> <statement>
	else
	{
		_DPRNR(1);
		CHECK_RULE(statement);
	}

	//Never executed!
    return RULE_OK;
}

static int prolog(ParserData* pd)
{
	//<prolog> -> <?php declare ( strict_types = 1 ) ; <prog> <end>
	RULE_OPEN;
	_DPRNR(0);


	int errtype = ERROR_SYNTAX;
	// if (!check_source_code_start())
	// 	return errtype;
	
	//scanner_get_next_token(&pd->token);

	GET_N_CHECK_TYPE(prolog);

	GET_N_CHECK_TYPE(ID);
	if (!str_cmp(pd->token.value.String, "declare"))
		return errtype;
	
	GET_N_CHECK_TYPE(left_bracket);
	GET_N_CHECK_TYPE(ID);
	if (!str_cmp(pd->token.value.String, "strict_types"))
		return errtype;
		
	GET_N_CHECK_TYPE(equal_sign);
	GET_N_CHECK_TYPE(integer);
	if (pd->token.value.integer != 1)
		return errtype;

	GET_N_CHECK_TYPE(right_bracket);
	GET_N_CHECK_TYPE(semicolon);

	CHECK_RULE(prog);
	
	CHECK_RULE(end);

    return RULE_OK;
}

int parser_parse()
{
    ParserData pd;
    init_data(&pd);
    //int result;
    // if ((result = scanner_get_next_token(&pd.token)) == TOKEN_OK)
    // {
    int result = prolog(&pd);
    //}

    return result;
}