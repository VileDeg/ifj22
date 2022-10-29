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

#define token_str pd->token.value.String->ptr

#ifdef IFJ22_DEBUG
	#define _DPRNR(_n)\
		do{\
			set_debug_out(get_pars_out());\
			DEBUGPR("Current token: ");\
			debug_print_token(pd->token);\
			DEBUGPR("%s -> %s\n", __func__, get_rule_expansion_by_name(__func__, (_n)));\
		}while(0)
	#define GET_NEXT_TOKEN()\
		do {\
			if (!pd->last_rule_was_eps) {\
				if ((RES = scanner_get_next_token(&pd->token)) != TOKEN_OK)\
					return RES;\
				set_debug_out(get_scan_out());\
				debug_print_token(pd->token);\
			} else\
				pd->last_rule_was_eps = false;\
		}while(0)
#else
	#define _DPRNR(_n)
	#define GET_NEXT_TOKEN()\
		do {\
			if (!pd->last_rule_was_eps) {\
				if ((RES = scanner_get_next_token(&pd->token)) != TOKEN_OK)\
					return RES;\
			} else\
				pd->last_rule_was_eps = false;\
		}while(0)
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


#define HAS_DOLLAR (token_str[0] == '$')

#define IS_VAR_ID (TYPE_IS(ID) && HAS_DOLLAR)

#define CHECK_DOLLAR if (!HAS_DOLLAR) return ERROR_SYNTAX

#define GET_N_CHECK_VAR_ID GET_N_CHECK_TYPE(ID); CHECK_DOLLAR

#define FUNC_ID_IS(_id) (TYPE_IS(ID) && str_cmp(pd->token.value.String, "reads"))

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// #define ST_GLOBAL 0
// #define ST_LOCAL 1

#define GLOBAL_TABLE 1
#define LOCAL_TABLE  0
#define ADD_NEW_ID(_dst)\
	do{\
		bool _err = false;\
		if (!pd->in_local_scope)\
			pd->_dst = symtable_add_symbol(pd->in_local_scope ? &pd->localTable : &pd->globalTable, token_str, &_err);\
		if (_err) return ERROR_INTERNAL;\
	}while(0)
// #define ADD_ID_TO(_global)\
// 	do{\
// 		bool err = false;\
// 		if (_global)\
// 			pd->rhs_func = symtable_add_symbol(&pd->globalTable, token_str, &err);\
// 		else\
// 			pd->lhs_var = symtable_add_symbol(&pd->localTable, token_str, &err);\
// 		if (err) return ERROR_INTERNAL;\
// 	}while(0)
//#define FOUND_ID_IN(_table) symtable_find(&pd->_table, token_str)
#define ID_FOUND symtable_find(pd->in_local_scope ? &pd->localTable : &pd->globalTable, token_str)
	
	// do{\
	// 	if (_global) {\
	// 		if (!(pd->rhs_func = symtable_find(&pd->globalTable, token_str))) return ERROR_INTERNAL;\
	// 	} else\
	// 		if (!(pd->lhs_var = symtable_find(&pd->localTable, token_str))) return ERROR_INTERNAL;\
	// }while(0)
///////////////////////////////
static FILE* s_CodegenOut = NULL;
#ifdef IFJ22_DEBUG
#define CODEGEN(_funcptr, ...) do { if (!_funcptr(__VA_ARGS__)) return ERROR_INTERNAL;\
	IFJ22_ASSERT(s_CodegenOut, "Code generator output file not found");\
	code_generator_flush(s_CodegenOut); }while(0)
#else
#define CODEGEN(_funcptr, ...) if (!_funcptr(__VA_ARGS__)) return ERROR_INTERNAL
#endif //IFJ22_DEBUG

////////////////////////////////////////////
#define EPSRULE pd->last_rule_was_eps = true
/////////////////////////////
GENERATE_STACK_DEFINITION(Token, tk)
GENERATE_STACK_DEFINITION(str_t, str)

static tkstack_t  s_TokenStack;
static strstack_t s_StringStack;

static bool init_data(ParserData* pd)
{
    symtable_init(&pd->globalTable);
	symtable_init(&pd->localTable);

	pd->in_param_list = pd->last_rule_was_eps = pd->in_local_scope = false;
	pd->rhs_func = pd->lhs_var = NULL;
	pd->param_index = 0;

	tkstack_init(&s_TokenStack);
	strstack_init(&s_StringStack);

	TData* data = NULL;
	bool err = false;
	if (!(data = symtable_add_symbol(&pd->globalTable, "reads", &err)))
		return false;
	data->type = TYPE_STRING;

	if (!(data = symtable_add_symbol(&pd->globalTable, "readi", &err)))
		return false;
	data->type = TYPE_INT;

	if (!(data = symtable_add_symbol(&pd->globalTable, "readf", &err)))
		return false;
	data->type = TYPE_FLOAT;

	if (!(data = symtable_add_symbol(&pd->globalTable, "write", &err)))
		return false;

	if (!(data = symtable_add_symbol(&pd->globalTable, "strlen", &err)))
		return false;
	data->type = TYPE_INT;
	if (!symtable_add_param(data, TYPE_STRING)) return false;

	if (!(data = symtable_add_symbol(&pd->globalTable, "substring", &err)))
		return false;
	data->type = TYPE_STRING;
	if (!symtable_add_param(data, TYPE_STRING)) return false;
	if (!symtable_add_param(data, TYPE_INT)) return false;
	if (!symtable_add_param(data, TYPE_INT)) return false;
	
	if (!(data = symtable_add_symbol(&pd->globalTable, "ord", &err)))
		return false;
	if (!symtable_add_param(data, TYPE_STRING)) return false;

	if (!(data = symtable_add_symbol(&pd->globalTable, "chr", &err)))
		return false;
	if (!symtable_add_param(data, TYPE_INT)) return false;


	if (!(data = symtable_add_symbol(&pd->globalTable, "$EXPR_REG", &err)))
		return false;

	return true;
}

static void free_data(ParserData* pd)
{
	symtable_clear(&pd->globalTable);
	symtable_clear(&pd->localTable);
}

static int type			 (ParserData* pd);
static int term			 (ParserData* pd);
static int assign		 (ParserData* pd);
static int rvalue		 (ParserData* pd);
static int statement	 (ParserData* pd);
static int func_type	 (ParserData* pd);
static int args			 (ParserData* pd);
static int arg_n		 (ParserData* pd);
static int params		 (ParserData* pd);
static int param_n		 (ParserData* pd);
static int program		 (ParserData* pd);
static int begin		 (ParserData* pd);
static int compound_stat (ParserData* pd);
static int condition_stat(ParserData* pd);
static int end			 (ParserData* pd);

static int end(ParserData* pd)
{
	RULE_OPEN;
	{
		//<end> -> ?>
		GET_NEXT_TOKEN();

		if (TYPE_IS(end))
			_DPRNR(0);
		//<end> -> ε
		else
			_DPRNR(1);
	}
	return RULE_OK;
}

static int term(ParserData* pd)
{
	#define CHECK_FUNC_PARAM_TYPE(_typelett)\
		if (pd->rhs_func->params->ptr[pd->param_index] != _typelett)\
			return ERROR_SEM_FUNC_COMPAT;

	RULE_OPEN;
	{
		if (strcmp(pd->rhs_func->id, "write")) // if function not "write"
		{
			if (pd->rhs_func->params->len == pd->param_index)
				return ERROR_SEM_FUNC_COMPAT;

			CODEGEN(emit_function_pass_param, pd->token, pd->param_index);
		}
		else 
		{
			if (IS_VAR_ID && !symtable_find(&pd->localTable, token_str))
				return ERROR_SEM_UNDEF_VAR;

			// we need to store args to stack to pass them in inverse order
			tkstack_push(&s_TokenStack, pd->token);
			//char* tkstr = calloc(1, pd->token.value.String->len+1);
			str_t tkstr;
			str_const(&tkstr);
			str_concat(&tkstr, token_str);
			strstack_push(&s_StringStack, tkstr);

			goto end;
		}

		//<term> -> INT_VALUE
		if (TYPE_IS(integer))
		{
			_DPRNR(0);

			CHECK_FUNC_PARAM_TYPE('i');
		}
		//<term> -> FLOAT_VALUE
		else if (TYPE_IS(float))
		{
			_DPRNR(1);

			CHECK_FUNC_PARAM_TYPE('f');
		}
		//<term> -> STRING_VALUE
		else if (TYPE_IS(string))
		{
			_DPRNR(2);

			CHECK_FUNC_PARAM_TYPE('s');
		}
		//<term> -> $ID
		else if (IS_VAR_ID)
		{
			_DPRNR(3);

			TData* var = symtable_find(&pd->localTable, token_str);
			if (!var) 
				return ERROR_SEM_UNDEF_VAR;
			switch (var->type)
			{
			case TYPE_INT:
				CHECK_FUNC_PARAM_TYPE('i');
				break;
			case TYPE_FLOAT:
				CHECK_FUNC_PARAM_TYPE('f');
				break;
			case TYPE_STRING:
				CHECK_FUNC_PARAM_TYPE('s');
			default: // Unsupported or unspecified type, shouldn't get here
				return ERROR_INTERNAL;
			}
		}
		else
			return ERROR_SYNTAX;
end:		
		pd->param_index++;
	}
	return RULE_OK;
}

static int args(ParserData* pd)
{
	RULE_OPEN;
	{
		pd->param_index = 0;

		//<args> -> <term> <arg_n>
		if (RULE_GOOD(term))
		{
			_DPRNR(0);
			CHECK_RULE(arg_n);

			if (!strcmp(pd->rhs_func->id, "write")) // if function is "write"
			{
				while (!tkstack_empty(&s_TokenStack) && !strstack_empty(&s_StringStack))
				{
					Token tk = tkstack_top(&s_TokenStack);
					str_t tk_str = strstack_top(&s_StringStack);

					tk.value.String = &tk_str;

					CODEGEN(emit_function_pass_param_push, tk);
					
					tkstack_pop(&s_TokenStack);
					str_dest(&tk_str);
					strstack_pop(&s_StringStack);
				}
				// We need to define argument count for variable number of arguments
				CODEGEN(emit_function_pass_param_count, pd->param_index);
			}
		}
		//<args> -> ε
		else
		{
			_DPRNR(1);
			EPSRULE;
		}

	}
	return RULE_OK;
}

static int arg_n(ParserData* pd)
{
	RULE_OPEN;
	{
		GET_NEXT_TOKEN();

		//<arg_n> -> , <term> <arg_n>
		if (TYPE_IS(comma))
		{
			_DPRNR(0);
			CHECK_RULE(term);
			CHECK_RULE(arg_n);
		}
		//<arg_n> -> eps
		else
		{
			_DPRNR(1);
			EPSRULE;
		}
	}
	return RULE_OK;
}

static int type(ParserData* pd)
{
	#define NUM_F_TYPES 3
	RULE_OPEN;
	{
		//<type> -> float
		//<type> -> int
		//<type> -> string
		//<type> -> ? float
		//<type> -> ? int
		//<type> -> ? string

		static const char* type_kw	 [NUM_F_TYPES] = { "float", "int", "string" 		 };
		static const int   type_macro[NUM_F_TYPES] = { TYPE_FLOAT, TYPE_INT, TYPE_STRING };
		
		CHECK_TYPE(keyword);
		
		int _rulenr = 0; // for debugging

		const char* str = token_str;
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
					if (!symtable_add_param(pd->rhs_func, type_macro[i]))
						return ERROR_INTERNAL;
				}
				else // If it's the return type of the current func.
					pd->rhs_func->type = type_macro[i];
			}
		}
		if (!matched)
			return ERROR_SYNTAX;
	}
	return RULE_OK;
}

static int rvalue(ParserData* pd)
{
	RULE_OPEN;
	{
		//GET_NEXT_TOKEN();

		//<rvalue> -> ID ( <args> )
		if (TYPE_IS(ID))
		{
			_DPRNR(0);

			{ //Check if function was defined
				if ((pd->rhs_func = symtable_find(&pd->globalTable, token_str)))
				{
					if (pd->lhs_var)
					{
						pd->lhs_var->type = pd->rhs_func->type;
						// if (pd->lhs_var->type != pd->rhs_func->type)
						// {
						// 	if (pd->lhs_var->type == TYPE_STRING || pd->rhs_func->type == TYPE_STRING) 
						// 		return ERROR_SEM_FUNC_COMPAT; // Cannot implicitly convert
						// }
					}

					CODEGEN(emit_function_before_pass_params);

					GET_N_CHECK_TYPE(left_bracket);
					{
						GET_NEXT_TOKEN();
						CHECK_RULE(args);
					}
					GET_N_CHECK_TYPE(right_bracket);

					if (strcmp(pd->rhs_func->id, "write") && pd->rhs_func->params->len != pd->param_index)
						return ERROR_SEM_FUNC_COMPAT;
					
					CODEGEN(emit_function_call, pd->rhs_func->id);
					if (pd->lhs_var)
					{
						CODEGEN(emit_function_res_assign, pd->lhs_var->id, pd->lhs_var->type, pd->rhs_func->type);
					}
					
				}
				else
				{
					//TODO: if not YET defined, add temporary entry to symtable
					return ERROR_SEM_ID_DEF;
				}
			}

			
		}
		//<rvalue> -> <expression>
		else
		{
			_DPRNR(1);

			/*TODO: if */ expression_parsing(pd); //etc...
		}
	}
	return RULE_OK;
}

static int assign(ParserData* pd)
{
	RULE_OPEN;
	{
		//<assign> -> = <rvalue>
		if (TYPE_IS(equal_sign))
		{
			_DPRNR(0);
			GET_NEXT_TOKEN();
			CHECK_RULE(rvalue);
		}
		//<assign> -> eps
		else
		{
			_DPRNR(1);
			EPSRULE;
		}
	}
	return RULE_OK;
}

static int compound_stat(ParserData* pd)
{
	RULE_OPEN;
	{
		//<compound_stat> -> { <statement> }
		_DPRNR(0);

		GET_N_CHECK_TYPE(left_curly_bracket);
		{
			GET_NEXT_TOKEN();
			CHECK_RULE(statement);
		}
		GET_N_CHECK_TYPE(right_curly_bracket);
	}
	return RULE_OK;
}

static int condition_stat(ParserData* pd)
{
	RULE_OPEN;
	{
		//<condition_stat> -> ( <expression> ) <compound_stat>
		_DPRNR(0);
		GET_N_CHECK_TYPE(left_bracket);
		{
			/*TODO: if */ expression_parsing(pd); //etc...
		}
		GET_N_CHECK_TYPE(right_bracket);

		CHECK_RULE(compound_stat);
	}
	return RULE_OK;
}

static int statement(ParserData* pd)
{
	RULE_OPEN;
	{
		//<statement> -> $ID <assign>
		if (IS_VAR_ID)
		{
			_DPRNR(0);

			{
				if (!ID_FOUND) 
					ADD_NEW_ID(lhs_var);
			}

			{ CODEGEN(emit_define_var, pd->lhs_var->id, pd->in_local_scope); }

			GET_NEXT_TOKEN();
			CHECK_RULE(assign);

			GET_N_CHECK_TYPE(semicolon);
		}
		//<statement> -> if <condition_stat> else <compound_stat>
		else if (KEYWORD_IS(if))
		{
			_DPRNR(1);
			CHECK_RULE(condition_stat);

			GET_N_CHECK_KEYWORD(else);

			CHECK_RULE(compound_stat);

			GET_NEXT_TOKEN();
			CHECK_RULE(statement);
		}
		//<statement> -> while <condition_stat>
		else if (KEYWORD_IS(while))
		{
			_DPRNR(2);
			CHECK_RULE(condition_stat);

			GET_NEXT_TOKEN();
			CHECK_RULE(statement);
		}
		//<statement> -> return <expression> ;
		else if (KEYWORD_IS(return))
		{
			_DPRNR(3);
			/*TODO: if */ expression_parsing(pd); //etc...
			GET_N_CHECK_TYPE(semicolon);
		}
		//<statement> -> <rvalue>
		else if (RULE_GOOD(rvalue))
		{
			_DPRNR(4);
			GET_N_CHECK_TYPE(semicolon);
		}
		//<statement> -> ε
		else
		{
			_DPRNR(5);
			EPSRULE;
		}
	}
	return RULE_OK;
}

static int func_type(ParserData* pd)
{
	RULE_OPEN;
	{
		pd->in_param_list = false; // <-- referenced in 'type' rule

		GET_NEXT_TOKEN();
		
		//<func_type> -> void
		if (KEYWORD_IS(void))
		{
			_DPRNR(0);

			//pd->rhs_func->type = TYPE_NIL;
		}
		//<func_type> -> <type>
		else
		{
			_DPRNR(1);
			CHECK_RULE(type);
		}
	}
	return RULE_OK;
}

static int param_n(ParserData* pd)
{
	RULE_OPEN;
	{
		GET_NEXT_TOKEN();

		//<param_n> -> , <type> $ ID <param_n>
		if (TYPE_IS(comma))
		{
			_DPRNR(0);

			CHECK_RULE(type);

			GET_N_CHECK_VAR_ID;
			{ //Add var to local table
				if (ID_FOUND)
					return ERROR_SEM_ID_DEF;
				ADD_NEW_ID(lhs_var);
			}
			
			pd->param_index++;
			{ CODEGEN(emit_function_param_declare, token_str, pd->param_index); }

			GET_NEXT_TOKEN();
			CHECK_RULE(param_n);
		}
		//<param_n> -> eps
		else
			_DPRNR(0);
	}
	return RULE_OK;
}

static int params(ParserData* pd)
{
	RULE_OPEN;
	{
		pd->in_param_list = true;

		GET_NEXT_TOKEN();

		//<params> -> <type> $ ID <param_n>
		if (RULE_GOOD(type))
		{
			_DPRNR(0);

			GET_N_CHECK_VAR_ID;
			{ //Add var to local table
				if (ID_FOUND)
					return ERROR_SEM_ID_DEF;
				ADD_NEW_ID(lhs_var);
			}
			
			{ CODEGEN(emit_function_param_declare, token_str, pd->param_index); }

			GET_NEXT_TOKEN();
			CHECK_RULE(param_n);
		}
		else
			_DPRNR(1);
		//<params> -> eps
	}
	return RULE_OK;
}

static int program(ParserData* pd)
{
	RULE_OPEN;

    //<program> -> function ID ( <params> ) : <func_type> <compound_stat> <program>
	GET_NEXT_TOKEN();
    if (KEYWORD_IS(function))
    {
		_DPRNR(0);
		
		GET_N_CHECK_TYPE(ID);
		{ //Add func id to global table
			if (ID_FOUND) //Function already defined
				return ERROR_SEM_ID_DEF;
			ADD_NEW_ID(rhs_func);
		}		

		pd->in_local_scope = true;

		GET_N_CHECK_TYPE(left_bracket);
		{
			CHECK_RULE(params);
		}
		GET_N_CHECK_TYPE(right_bracket);
		GET_N_CHECK_TYPE(colon);

		CHECK_RULE(func_type);
		
		{ CODEGEN(emit_function_open, token_str); }

		CHECK_RULE(compound_stat);

		
		{ CODEGEN(emit_function_close, token_str); }
		
		pd->in_local_scope = false;
		CHECK_RULE(program);
    }
	//<program> -> <statement> <program>
	else
	{
		_DPRNR(1);
		CHECK_RULE(statement);
		CHECK_RULE(program);
	}

	
    return RULE_OK;
}

static int begin(ParserData* pd)
{
	//<begin> -> <?php declare ( strict_types = 1 ) ; <program> <end>
	RULE_OPEN;
	{
		
		
		{
			GET_N_CHECK_TYPE(prologue);
			_DPRNR(0);
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

		//{ CODEGEN(emit_body_open); }

		CHECK_RULE(program);
		
		CHECK_RULE(end);
	}
    return RULE_OK;
}

int parse_file(FILE* fptr)
{
	str_t string;
    ParserData pd;
    if (!str_const(&string) || !init_data(&pd) || !code_generator_init())
		goto error;

	scanner_set_file(fptr);
	scanner_set_string(&string);

	s_CodegenOut = stdout;

    int result = begin(&pd);
	code_generator_flush(s_CodegenOut);
	code_generator_terminate();

	goto free;

error:
	result = ERROR_INTERNAL;
free:
	str_dest(&string);
	free_data(&pd);
end:
    return result;
}
