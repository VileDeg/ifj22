#include <string.h>

#include "parser.h"
#include "errors.h"
#include "debug.h"
#include "scanner.h"
#include "symtable.h"
#include "precedence_t.h"
#include "code_generator.h"

#define RES result
#define DEF_RES int RES
#define RULE_OPEN DEF_RES

#ifdef IFJ22_DEBUG
	#define _DPRNR(_n)\
		do{\
			set_debug_out(get_pars_out());\
			DEBUGPR("%s -> %s\n", __func__, get_rule_expansion_by_name(__func__, (_n)));\
		}while(0)
	#define GET_NEXT_TOKEN()\
		do {\
			if ((RES = scanner_get_next_token(&pd->token)) != TOKEN_OK)\
				return RES;\
			set_debug_out(get_scan_out());\
			debug_print_token(pd->token);\
		}while(0)
#else
	#define _DPRNR(_n)
	#define GET_NEXT_TOKEN()\
		if ((RES = scanner_get_next_token(&pd->token)) != TOKEN_OK)\
			return RES
#endif

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

#define FUNC_ID_IS(_id) (TYPE_IS(ID) && str_cmp(pd->token.value.String, "reads"))

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// #define ST_GLOBAL 0
// #define ST_LOCAL 1
#define ADD_SYMBOL\
	do{\
		bool err = false;\
		if (!pd->in_scope)\
			pd->current_func = symtable_add_symbol(&pd->globalTable, pd->token.value.String->ptr, &err);\
		else\
			pd->current_var = symtable_add_symbol(&pd->localTable, pd->token.value.String->ptr, &err);\
		if (err) return ERROR_INTERNAL;\
	}while(0)

static bool init_data(ParserData* pd)
{
    symtable_init(&pd->globalTable);
	symtable_init(&pd->localTable);

	pd->in_scope = pd->in_param_list = false;
	pd->current_func = pd->current_var = NULL;

	return true;
}

static void free_data(ParserData* pd)
{
	symtable_clear(&pd->globalTable);
	symtable_clear(&pd->localTable);
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
static int args(ParserData* pd);
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
	//printf("<%s> -> ???\n", __func__);
	//TODO: 
	expression_parsing(pd);
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

#define NUM_F_TYPES 3

static int type(ParserData* pd)
{
	RULE_OPEN;
	//<type> -> float
	//<type> -> int
	//<type> -> string
	//<type> -> ? float
	//<type> -> ? int
	//<type> -> ? string
	
	static const char* type_kw[NUM_F_TYPES] = { "float", "int", "string" };
	static const int   type_macro[NUM_F_TYPES] = { TYPE_FLOAT, TYPE_INT, TYPE_STRING };
	
	CHECK_TYPE(keyword);
	
	int _rulenr = 0; // for debug

	const char* str = pd->token.value.String->ptr;
	if (str[0] == '?')
	{
		//Type can be null!
		++str;
		_rulenr += 3;
	}
	bool matched = false;
	for (int i = 0; i < NUM_F_TYPES; i++)
	{
		if (!strcmp(str, type_kw[i]))
		{
			_DPRNR(_rulenr+i);
			
			matched = true;
			if (pd->in_param_list) // If it's the type for func. parameter
			{
				if (!symtable_add_param(pd->current_func, type_macro[i]))
					return ERROR_INTERNAL;
			}
			else // If it's the return type of the current func.
				pd->current_func->type = type_macro[i];
		}
	}
	if (!matched)
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

#define RHS_VALUE_FUNC_ARGS GET_N_CHECK_TYPE(left_bracket); GET_N_CHECK_RULE(args); GET_N_CHECK_TYPE(right_bracket);

static int rhs_value(ParserData* pd)
{
	RULE_OPEN;

	//GET_N_CHECK_TYPE...
	//DEBUGPR("%s -> ???\n", __func__);

	//Will be looked up in symtable
	//<rhs_value> -> ID ( <args> )
	if (TYPE_IS(ID))
	{
		_DPRNR(0);
		RHS_VALUE_FUNC_ARGS;
	}
	//<rhs_value> -> reads ( <args> )
	else if (FUNC_ID_IS("reads"))
	{
		_DPRNR(1);
		RHS_VALUE_FUNC_ARGS;
	}
	//<rhs_value> -> readi ( <args> )
	else if (FUNC_ID_IS("readi"))
	{
		_DPRNR(2);
		RHS_VALUE_FUNC_ARGS;
	}
	//<rhs_value> -> readf ( <args> )
	else if (FUNC_ID_IS("readf"))
	{
		_DPRNR(3);
		RHS_VALUE_FUNC_ARGS;
	}
	//<rhs_value> -> write ( <args> )
	else if (FUNC_ID_IS("write"))
	{
		_DPRNR(4);
		RHS_VALUE_FUNC_ARGS;
	}
	//<rhs_value> -> floatval ( <args> )
	else if (FUNC_ID_IS("floatval"))
	{
		_DPRNR(5);
		RHS_VALUE_FUNC_ARGS;
	}
	//<rhs_value> -> intval ( <args> )
	else if (FUNC_ID_IS("intval"))
	{
		_DPRNR(6);
		RHS_VALUE_FUNC_ARGS;
	}
	//<rhs_value> -> strval ( <args> )
	else if (FUNC_ID_IS("strval"))
	{
		_DPRNR(7);
		RHS_VALUE_FUNC_ARGS;
	}
	//<rhs_value> -> strlen ( <args> )
	else if (FUNC_ID_IS("strlen"))
	{
		_DPRNR(8);
		RHS_VALUE_FUNC_ARGS;
	}
	//<rhs_value> -> substring ( <args> )
	else if (FUNC_ID_IS("substring"))
	{
		_DPRNR(9);
		RHS_VALUE_FUNC_ARGS;
	}
	//<rhs_value> -> ord ( <args> )
	else if (FUNC_ID_IS("ord"))
	{
		_DPRNR(10);
		RHS_VALUE_FUNC_ARGS;
	}
	//<rhs_value> -> chr ( <args> )
	else if (FUNC_ID_IS("chr"))
	{
		_DPRNR(11);
		RHS_VALUE_FUNC_ARGS;
	}
	//<rhs_value> -> <expression>
	else
	{
		_DPRNR(12);
		CHECK_RULE(expression);
	}

	return RULE_OK;
}

static int def_var(ParserData* pd)
{
	RULE_OPEN;

	//<def_var> -> = <rhs_value>
	if (TYPE_IS(equal_sign))
	{
		_DPRNR(0);
		GET_N_CHECK_RULE(rhs_value);
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

		{
			ADD_SYMBOL;
		}

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

	pd->in_param_list = false;

	//<rettype> -> void
	GET_NEXT_TOKEN();
	if (is_void_type(pd))
	{
		_DPRNR(0);

		pd->current_func->type = TYPE_NIL;
	}
	else
	{
		//<rettype> -> <type>
		_DPRNR(1);
		CHECK_RULE(type);
	}

	return RULE_OK;
}

static int args(ParserData* pd)
{
	RULE_OPEN;

	//<args> -> <value> <arg_n>
	if (RULE_GOOD(value))
	{
		_DPRNR(0);
		GET_N_CHECK_RULE(arg_n);
	}
	else
		_DPRNR(1);

	//<args> -> ε

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
		{ //Add var to local table
			ADD_SYMBOL;
		}	

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

	pd->in_param_list = true;

	//<params> -> <type> $ ID <param_n>
	if (RULE_GOOD(type))
	{
		_DPRNR(0);

		GET_N_CHECK_VAR_ID;
		{ //Add var to local table
			ADD_SYMBOL;
		}	

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

		pd->in_scope = true;
		
		GET_N_CHECK_TYPE(ID);

		{ //Add func id to global table
			ADD_SYMBOL;
		}		

		GET_N_CHECK_TYPE(left_bracket);

		GET_N_CHECK_RULE(params);
		
		GET_N_CHECK_TYPE(right_bracket);
		GET_N_CHECK_TYPE(colon);

		CHECK_RULE(rettype);

		GET_N_CHECK_RULE(scoped_stat);

		pd->in_scope = false;
		
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

	GET_N_CHECK_TYPE(prolog);

	{
		GET_N_CHECK_TYPE(ID);
		if (!str_cmp(pd->token.value.String, "declare"))
			return ERROR_SYNTAX;
		
		GET_N_CHECK_TYPE(left_bracket);
		GET_N_CHECK_TYPE(ID);
		if (!str_cmp(pd->token.value.String, "strict_types"))
			return ERROR_SYNTAX;
			
		GET_N_CHECK_TYPE(equal_sign);
		GET_N_CHECK_TYPE(integer);
		if (pd->token.value.integer != 1)
			return ERROR_SYNTAX;

		GET_N_CHECK_TYPE(right_bracket);
		GET_N_CHECK_TYPE(semicolon);
	}

	CHECK_RULE(prog);
	
	CHECK_RULE(end);

    return RULE_OK;
}

int parse_file(FILE* fptr)
{
	str_t string;
    ParserData pd;
    if (!str_const(&string) || !init_data(&pd) || !code_generator_start())
		goto error;

	scanner_set_file(fptr);
	scanner_set_string(&string);

    int result = prolog(&pd);
	code_generator_flush(stdout);

	goto free;

error:
	result = ERROR_INTERNAL;
free:
	str_dest(&string);
	free_data(&pd);
end:
    return result;
}



int parse_old()
{
    ParserData pd;
    if (!init_data(&pd))
		goto error;
    
    int result = prolog(&pd);

error:
	result = ERROR_INTERNAL;
free:
	free_data(&pd);
end:
    return result;
}