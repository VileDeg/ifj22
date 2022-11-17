#include <string.h>

#include "parser.h"
#include "errors.h"
#include "scanner.h"
#include "symtable.h"
#include "precedence_t.h"
#include "code_generator.h"

#define RES result
#define DEF_RES int64_t RES
#define RULE_OPEN  DEF_RES
#define RULE_CLOSE return SUCCESS;

#ifdef IFJ22_DEBUG
	#define _DPRNR(_n)\
		do{\
			if (g_DebugOn)\
			{\
				set_debug_out(get_pars_out());\
				DEBUGPR("Current token: ");\
				debug_print_token(pd->token);\
				DEBUGPR("%s -> %s\n", __func__, get_rule_expansion_by_name(__func__, (_n)));\
			}\
		}while(0)
	#define GET_NEXT_TOKEN()\
		do {\
			if (!pd->block_next_token) {\
				if ((RES = scanner_get_next_token(&pd->token)) != SUCCESS)\
					return RES;\
				if (g_DebugOn) { set_debug_out(get_scan_out());\
					debug_print_token(pd->token); }\
			} else\
				pd->block_next_token = false;\
		}while(0)
#else
	#define _DPRNR(_n)
	#define GET_NEXT_TOKEN()\
		do {\
			if (!pd->block_next_token) {\
				if ((RES = scanner_get_next_token(&pd->token)) != SUCCESS)\
					return RES;\
			} else\
				pd->block_next_token = false;\
		}while(0)
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if 1

#define TOKEN_IS(_postfix) (pd->token.type == token_##_postfix)

#define KEYWORD_IS(_kw) (TOKEN_IS(keyword) && pd->token.keyword == keyword_##_kw)

//Never use with more than 1 argument!
#define CHECK_TOKEN(_postfix, ...)\
	if (!TOKEN_IS(_postfix##__VA_ARGS__)) { return ERROR_SYNTAX; } else {}

//Never use with more than 1 argument!
#define CHECK_KEYWORD(_kw, ...)\
	if (!KEYWORD_IS(_kw##__VA_ARGS__)) { return ERROR_SYNTAX; } else {}

//Never use with more than 1 argument!
#define NEXT_TK_CHECK_TOKEN(_type, ...)\
	do {\
		GET_NEXT_TOKEN();\
		CHECK_TOKEN(_type##__VA_ARGS__);\
	} while(0)

//Never use with more than 1 argument!
#define NEXT_TK_CHECK_KEYWORD(_kw, ...)\
	do {\
		GET_NEXT_TOKEN();\
		CHECK_KEYWORD(_kw##__VA_ARGS__);\
	} while(0)

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define RULE_GOOD(_rule) (_rule(pd) == SUCCESS)


#define CHECK_RULE(_rule)\
	if ((RES = _rule(pd))) { return RES; } else {}

#define NEXT_TK_CHECK_RULE(_rule)\
	do {\
		GET_NEXT_TOKEN();\
		CHECK_RULE(_rule);\
	} while(0)

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define TK_STR(_tk) _tk.string->ptr

#define HAS_DOLLAR (TK_STR(pd->token)[0] == '$')

#define IS_VAR_ID  (TOKEN_IS(ID) && HAS_DOLLAR )
#define IS_FUNC_ID (TOKEN_IS(ID) && !HAS_DOLLAR)

#define CHECK_DOLLAR 	if (!HAS_DOLLAR) { return ERROR_SYNTAX; } else {}
#define CHECK_NO_DOLLAR if (HAS_DOLLAR ) { return ERROR_SYNTAX; } else {}

#define NEXT_TK_CHECK_VAR_ID  NEXT_TK_CHECK_TOKEN(ID); CHECK_DOLLAR
#define NEXT_TK_CHECK_FUNC_ID NEXT_TK_CHECK_TOKEN(ID); CHECK_NO_DOLLAR

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define FIND_ID(_id) symtable_find(pd->in_local_scope ? &pd->localTable : &pd->globalTable, _id)
#define FIND_CURRENT_ID FIND_ID(TK_STR(pd->token))

#define ADD_ID(_dst, _id)\
	do {\
		bool _err = false;\
		_dst = symtable_add_symbol(pd->in_local_scope ? &pd->localTable : &pd->globalTable, _id, &_err);\
		if (_err) INTERNAL_ERROR_RET;\
	} while(0)
#define ADD_CURRENT_ID(_dst) ADD_ID(_dst, TK_STR(pd->token))

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define BLOCK_NEXT_TOKEN pd->block_next_token = true
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define TOKEN_IS_TYPE_KEYWORD (KEYWORD_IS(float) 	 || KEYWORD_IS(int) || KEYWORD_IS(string))

#define TOKEN_IS_VALUE_TYPE   (TOKEN_IS_TYPE_KEYWORD || TOKEN_IS(null)  || TOKEN_IS(ID)	 	 )

#endif
GENERATE_STACK_DEFINITION(Token, tk)
GENERATE_STACK_DEFINITION(str_t, str)

static tkstack_t  s_TokenStack;
static strstack_t s_StringStack;

// static Token s_TokenBackup;
static str_t s_TkStrBackup;

static bool init_data(ParserData* pd)
{
    symtable_init(&pd->globalTable);
	symtable_init(&pd->localTable);

	pd->in_param_list   = pd->block_next_token  = 
	pd->in_local_scope  = pd->func_questionmark = 
	pd->var_not_yet_def = false;

	pd->rhs_func = pd->lhs_var = NULL;

	pd->param_index = 0;
	pd->label_deep = -1;
	pd->label_index = 0;

	//str_const(&pd->var_name);

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
	
	if (!(data = symtable_add_symbol(&pd->globalTable, "floatval", &err)))
		return false;
	data->type = TYPE_FLOAT;
	if (!symtable_add_param(data, TYPE_UNDEF, true)) return false;

	if (!(data = symtable_add_symbol(&pd->globalTable, "intval", &err)))
		return false;
	data->type = TYPE_INT;
	if (!symtable_add_param(data, TYPE_UNDEF, true)) return false;

	if (!(data = symtable_add_symbol(&pd->globalTable, "strval", &err)))
		return false;
	data->type = TYPE_STRING;
	if (!symtable_add_param(data, TYPE_UNDEF, true)) return false;

	if (!(data = symtable_add_symbol(&pd->globalTable, "strlen", &err)))
		return false;
	data->type = TYPE_INT;
	if (!symtable_add_param(data, TYPE_STRING, false)) return false;

	if (!(data = symtable_add_symbol(&pd->globalTable, "substring", &err)))
		return false;
	data->type = TYPE_STRING;
	if (!symtable_add_param(data, TYPE_STRING, false)) return false;
	if (!symtable_add_param(data, TYPE_INT, false)) return false;
	if (!symtable_add_param(data, TYPE_INT, false)) return false;
	
	if (!(data = symtable_add_symbol(&pd->globalTable, "ord", &err)))
		return false;
	if (!symtable_add_param(data, TYPE_STRING, false)) return false;

	if (!(data = symtable_add_symbol(&pd->globalTable, "chr", &err)))
		return false;
	if (!symtable_add_param(data, TYPE_INT, false)) return false;


	if (!(data = symtable_add_symbol(&pd->globalTable, "$EXPR_REG", &err)))
		return false;
	data->type = TYPE_BOOL;

	return true;
}

static void free_data(ParserData* pd)
{
	symtable_clear(&pd->globalTable);
	symtable_clear(&pd->localTable);
	//str_dest(&pd->var_name);

	str_dest(&s_TkStrBackup);
}

void push_current_token(ParserData* pd)
{
	tkstack_push(&s_TokenStack, pd->token);

	if (!TOKEN_IS(ID)) //<-- only ID should have string parameter
		return;

	str_t tkstr;
	str_const(&tkstr);
	str_concat(&tkstr, TK_STR(pd->token));
	strstack_push(&s_StringStack, tkstr);
}

void pop_current_token(ParserData* pd)
{
	str_dest(&s_TkStrBackup);

	IFJ22_ASSERT(!tkstack_empty(&s_TokenStack), "Should not happen?");
	pd->token = tkstack_top(&s_TokenStack);
	tkstack_pop(&s_TokenStack);

	if (!TOKEN_IS(ID)) //<-- only ID should have string parameter
		return;
	
	s_TkStrBackup = strstack_top(&s_StringStack);
	pd->token.string = &s_TkStrBackup;
	strstack_pop(&s_StringStack);
}

static int type		(ParserData* pd);
static int term		(ParserData* pd);
static int assign	(ParserData* pd);
static int rvalue	(ParserData* pd);
static int statement(ParserData* pd);
static int func_type(ParserData* pd);
static int args		(ParserData* pd);
static int arg_n	(ParserData* pd);
static int params	(ParserData* pd);
static int param_n	(ParserData* pd);
static int program	(ParserData* pd);
static int end		(ParserData* pd);
static int begin	(ParserData* pd);

static int term(ParserData* pd)
{
	#define LETTFLIP(_s) ('z' - (_s) + 'a')

	RULE_OPEN;
	{
		if (!strcmp(pd->rhs_func->id, "write")) // if function is "write"
		{
			if (IS_VAR_ID && !FIND_CURRENT_ID)
				PRINT_ERROR_RET(ERROR_SEM_UNDEF_VAR, "undefined variable passed as parameter.");

			// we need to store args to stack to pass them in inverse order
			push_current_token(pd);

			goto end;
		}

		if (pd->rhs_func->params->len == pd->param_index)
			PRINT_ERROR_RET(ERROR_SEM_TYPE_COMPAT, "invalid number of parameters passed.");

		{ CODEGEN(emit_function_pass_param, pd->token, pd->param_index, pd->in_local_scope); }

		if (!strcmp(pd->rhs_func->id, "floatval") || 
			!strcmp(pd->rhs_func->id, "intval"	) ||
			!strcmp(pd->rhs_func->id, "strval"  ))
			goto end;

		int func_param = pd->rhs_func->params->ptr[pd->param_index];
		int sign = '\0';
		switch (pd->token.type)
		{
			//<term> -> INT_VALUE
			case token_integer:
				_DPRNR(0);
				sign = 'i';	
				break;
			//<term> -> FLOAT_VALUE
			case token_float:
				_DPRNR(1);
				sign = 'f';	
				break;
			//<term> -> STRING_VALUE
			case token_string:
				_DPRNR(2);
				sign = 's';	
				break;
			//<term> -> NULL
			case token_null:
				_DPRNR(3);
				goto null;
				break;
			default:
				_DPRNR(4);

				if (!IS_VAR_ID)
					PRINT_ERROR_RET(ERROR_SEM_UNDEF_VAR, "unsupported variable type.");

				TData* var = FIND_CURRENT_ID;
				if (!var) 
					PRINT_ERROR_RET(ERROR_SEM_UNDEF_VAR, "undefined variable passed as parameter.");
				switch (var->type)
				{
					case TYPE_INT:
						sign = 'i';
						break;
					case TYPE_FLOAT:
						sign = 'f';
						break;
					case TYPE_STRING:
						sign = 's';
						break;
					case TYPE_NULL:
						goto null;
					default: // Unsupported or unspecified type, shouldn't get here
						INTERNAL_ERROR_RET;
				}
		}
		
		if (func_param != sign && func_param != LETTFLIP(sign))
	 		PRINT_ERROR_RET(ERROR_SEM_TYPE_COMPAT, "function parameter type incompatible.");
		goto end;
null:
		if (func_param != LETTFLIP('i') && func_param != LETTFLIP('f') && func_param != LETTFLIP('s'))
			PRINT_ERROR_RET(ERROR_SEM_TYPE_COMPAT, "function parameter type incompatible.");
end:		
		pd->param_index++;
	}
	RULE_CLOSE;
}

static int args(ParserData* pd)
{
	RULE_OPEN;
	{
		pd->param_index = 0;

		//<args> -> <term> <arg_n>
		if (TOKEN_IS_VALUE_TYPE)
		{
			_DPRNR(0);

			CHECK_RULE(term);
			NEXT_TK_CHECK_RULE(arg_n);

			if (!strcmp(pd->rhs_func->id, "write")) // if function is "write"
			{
				while (!tkstack_empty(&s_TokenStack) && !strstack_empty(&s_StringStack))
				{
					pop_current_token(pd);

					{ CODEGEN(emit_function_pass_param_push, pd->token, pd->in_local_scope); } //tk
				}
				// We need to define argument count for variable number of arguments
				{ CODEGEN(emit_function_pass_param_count, pd->param_index); }
			}
		}
		//<args> -> ε
		else
		{
			_DPRNR(1);
			BLOCK_NEXT_TOKEN;
		}
	}
	RULE_CLOSE;
}

static int arg_n(ParserData* pd)
{
	RULE_OPEN;
	{
		//GET_NEXT_TOKEN();

		//<arg_n> -> , <term> <arg_n>
		if (TOKEN_IS(comma))
		{
			_DPRNR(0);
			NEXT_TK_CHECK_RULE(term);
			NEXT_TK_CHECK_RULE(arg_n);
		}
		//<arg_n> -> eps
		else
		{
			_DPRNR(1);
			BLOCK_NEXT_TOKEN;
		}
	}
	RULE_CLOSE;
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

		static const int type_macro[NUM_F_TYPES] = { TYPE_FLOAT, TYPE_INT, TYPE_STRING };
		
		int _rulenr = 0; // for debugging

		if (pd->token.questionmark)
		{
			//Type can be null!
			pd->func_questionmark = true;
			_rulenr += 3;
		}
		else
			pd->func_questionmark = false;

		int  tnum = 0;
		if (KEYWORD_IS(float))
		{
			_DPRNR(_rulenr);
			tnum = 0;
		}
		else if (KEYWORD_IS(int))
		{
			_DPRNR(_rulenr+1);
			tnum = 1;
		}
		else if (KEYWORD_IS(string))
		{
			_DPRNR(_rulenr+2);
			tnum = 2;
		}
		else
			PRINT_ERROR_RET(ERROR_SYNTAX, "type mismatch.");

		if (pd->in_param_list) // If it's the type for func. parameter
		{
			if (!symtable_add_param(pd->rhs_func, type_macro[tnum], pd->token.questionmark))
				INTERNAL_ERROR_RET;
		}
		else // If it's the return type of the current func.
			pd->rhs_func->type = type_macro[tnum];
	}
	RULE_CLOSE;
}

static int rvalue(ParserData* pd)
{
	RULE_OPEN;
	{
		//<rvalue> -> ID ( <args> )
		if (IS_FUNC_ID)
		{
			_DPRNR(0);

			{ 
				//Check if function was defined
				if (!(pd->rhs_func = symtable_find(&pd->globalTable, TK_STR(pd->token))))
				{
					//If next token is not '(' then it's not a function call so we return syntax error
					NEXT_TK_CHECK_TOKEN(left_bracket);
					//TODO: if not YET defined, add temporary entry to symtable
					return ERROR_SEM_ID_DEF;
				}

				//If function is called as part of variable definition
				if (pd->lhs_var)
						pd->lhs_var->type = pd->rhs_func->type;

				{ CODEGEN(emit_function_before_pass_params); }

				NEXT_TK_CHECK_TOKEN(left_bracket);
				{
					NEXT_TK_CHECK_RULE(args);
				}
				NEXT_TK_CHECK_TOKEN(right_bracket);

				if (strcmp(pd->rhs_func->id, "write") && 
					pd->rhs_func->params->len != pd->param_index)
					return ERROR_SEM_TYPE_COMPAT;
				
				{ CODEGEN(emit_function_call, pd->rhs_func->id); }
				
				//If function is called as part of variable definition
				if (pd->lhs_var)
					{ CODEGEN(emit_function_res_assign, pd->lhs_var->id, pd->in_local_scope); }
			}
		}
		//<rvalue> -> <expression>
		else
		{
			_DPRNR(1);

			if (TOKEN_IS(right_curly_bracket))
				return ERROR_SYNTAX;

			push_current_token(pd);
			if ((RES = expression_parsing(pd)))
			{
				pop_current_token(pd);

				return RES;
			}
		}
	}
	RULE_CLOSE;
}

static int assign(ParserData* pd)
{
	RULE_OPEN;
	{
		//<assign> -> = <rvalue>
		if (TOKEN_IS(equal_sign))
		{
			_DPRNR(0);

			NEXT_TK_CHECK_RULE(rvalue);
		}
		//<assign> -> eps
		else
		{
			_DPRNR(1);
			//In this case variable is used as expression which means it has to be already defined
			if (!pd->lhs_var)
				return ERROR_SEM_UNDEF_VAR;
			BLOCK_NEXT_TOKEN;
		}
	}
	RULE_CLOSE;
}

static int func_type(ParserData* pd)
{
	RULE_OPEN;
	{
		pd->in_param_list = false; // <-- referenced in 'type' rule
		
		//<func_type> -> void
		if (KEYWORD_IS(void))
		{
			_DPRNR(0);

			pd->rhs_func->type = TYPE_NULL;
		}
		//<func_type> -> <type>
		else
		{
			_DPRNR(1);
			CHECK_RULE(type);
		}
	}
	RULE_CLOSE;
}

static int param_n(ParserData* pd)
{
	RULE_OPEN;
	{
		//<param_n> -> , <type> $ ID <param_n>
		if (TOKEN_IS(comma))
		{
			_DPRNR(0);

			NEXT_TK_CHECK_RULE(type);

			NEXT_TK_CHECK_VAR_ID;
			{ //Add var to local table
				if (FIND_CURRENT_ID)
					PRINT_ERROR_RET(ERROR_SEM_ID_DEF, "parameter already defined.");
				ADD_CURRENT_ID(pd->lhs_var);
			}
			
			pd->param_index++;
			{ CODEGEN(emit_function_param_declare, TK_STR(pd->token), pd->param_index); }

			NEXT_TK_CHECK_RULE(param_n);
		}
		//<param_n> -> eps
		else
		{
			_DPRNR(0);
			BLOCK_NEXT_TOKEN;
		}
	}
	RULE_CLOSE;
}

static int params(ParserData* pd)
{
	RULE_OPEN;
	{
		pd->in_param_list = true;

		//<params> -> <type> $ ID <param_n>
		if (TOKEN_IS_TYPE_KEYWORD)
		{
			_DPRNR(0);

			CHECK_RULE(type);
			NEXT_TK_CHECK_VAR_ID;
			{ //Add var to local table
				if (FIND_CURRENT_ID)
					PRINT_ERROR_RET(ERROR_SEM_ID_DEF, "parameter already defined.");
				ADD_CURRENT_ID(pd->lhs_var);
			}
			
			{ CODEGEN(emit_function_param_declare, TK_STR(pd->token), pd->param_index); }

			NEXT_TK_CHECK_RULE(param_n);
		}
		//<params> -> eps
		else
		{
			_DPRNR(1);
			BLOCK_NEXT_TOKEN;
		}
	}
	RULE_CLOSE;
}

static int statement(ParserData* pd)
{
	RULE_OPEN;
	{
		pd->lhs_var = NULL;
		if (!pd->in_local_scope)
			pd->rhs_func = NULL;

		//<statement> -> $ID <assign> ; <program>
		if (IS_VAR_ID)
		{
			_DPRNR(0);

			{
				if (!(pd->lhs_var = FIND_CURRENT_ID))
					ADD_CURRENT_ID(pd->lhs_var);

				{ CODEGEN(emit_define_var, pd->lhs_var->id, pd->in_local_scope); }

				pd->var_not_yet_def = true;

				NEXT_TK_CHECK_RULE(assign);

				pd->var_not_yet_def = false;
			}

			NEXT_TK_CHECK_TOKEN(semicolon);

			NEXT_TK_CHECK_RULE(program);
		}
		//<statement> -> if ( <expression> ) { <statement> } else { <statement> } <program>
		else if (KEYWORD_IS(if))
		{
			_DPRNR(1);

			NEXT_TK_CHECK_TOKEN(left_bracket);
			pd->lhs_var = symtable_find(&pd->globalTable, "$EXPR_REG");
			if (!pd->lhs_var)
				INTERNAL_ERROR_RET;

			pd->label_deep++;
			const char* func_name = pd->rhs_func ? pd->rhs_func->id : "";
			{ CODEGEN(emit_if_head); }

			CHECK_RULE(expression_parsing);

			{ CODEGEN(emit_if_open, func_name, pd->label_deep, pd->label_index); }

			NEXT_TK_CHECK_TOKEN(left_curly_bracket);
			{
				NEXT_TK_CHECK_RULE(statement);
			}
			NEXT_TK_CHECK_TOKEN(right_curly_bracket);

			NEXT_TK_CHECK_KEYWORD(else);

			{ CODEGEN(emit_else, func_name, pd->label_deep, pd->label_index); }

			NEXT_TK_CHECK_TOKEN(left_curly_bracket);
			{
				NEXT_TK_CHECK_RULE(statement);
			}
			NEXT_TK_CHECK_TOKEN(right_curly_bracket);

			{ CODEGEN(emit_if_close, func_name, pd->label_deep, pd->label_index + 1); }

			pd->label_index += 2;
			pd->label_deep--;

			NEXT_TK_CHECK_RULE(program);
		}
		//<statement> -> while ( <expression> ) { <statement> } <program>
		else if (KEYWORD_IS(while))
		{
			_DPRNR(2);
			
			NEXT_TK_CHECK_TOKEN(left_bracket);
			pd->lhs_var = symtable_find(&pd->globalTable, "$EXPR_REG");
			if (!pd->lhs_var)
				INTERNAL_ERROR_RET;

			pd->label_deep++;
			const char* func_name = pd->rhs_func ? pd->rhs_func->id : "";
			{ CODEGEN(emit_while_head, func_name, pd->label_deep, pd->label_index); }

			CHECK_RULE(expression_parsing);

			{ CODEGEN(emit_while_open, func_name, pd->label_deep, pd->label_index + 1); }

			NEXT_TK_CHECK_TOKEN(left_curly_bracket);
			{
				NEXT_TK_CHECK_RULE(statement);
			}
			NEXT_TK_CHECK_TOKEN(right_curly_bracket);

			{ CODEGEN(emit_while_close, func_name, pd->label_deep, pd->label_index + 1); }

			pd->label_index += 2;
			pd->label_deep--;

			NEXT_TK_CHECK_RULE(program);
		}
		//<statement> -> return <expression> ; <program>
		else if (KEYWORD_IS(return))
		{
			_DPRNR(3);

			NEXT_TK_CHECK_RULE(expression_parsing);

			{ CODEGEN(emit_clear_stack); } //<-- get rid of return value on stack

			NEXT_TK_CHECK_TOKEN(semicolon);

			NEXT_TK_CHECK_RULE(program);
		}
		//<statement> -> <rvalue> ; <program>
		else if (TOKEN_IS_VALUE_TYPE) //RULE_GOOD(rvalue)
		{
			_DPRNR(4);

			CHECK_RULE(rvalue);
			NEXT_TK_CHECK_TOKEN(semicolon);

			NEXT_TK_CHECK_RULE(program);
		}
		//<statement> -> ε
		else
		{
			_DPRNR(5);
			BLOCK_NEXT_TOKEN;
		}
	}
	RULE_CLOSE;
}

static int program(ParserData* pd)
{
	RULE_OPEN;
	{
		//<program> -> function ID ( <params> ) : <func_type> { <statement> } <program>
		if (KEYWORD_IS(function))
		{
			_DPRNR(0);
			
			NEXT_TK_CHECK_FUNC_ID;
			{ //Add func id to global table
				if (FIND_CURRENT_ID) //Function already defined
					PRINT_ERROR_RET(ERROR_SEM_ID_DEF, "function is already defined.");
				ADD_CURRENT_ID(pd->rhs_func);
			}		

			pd->in_local_scope = true;

			{ CODEGEN(emit_function_open, pd->rhs_func->id); }

			NEXT_TK_CHECK_TOKEN(left_bracket);
			{
				NEXT_TK_CHECK_RULE(params);
			}
			NEXT_TK_CHECK_TOKEN(right_bracket);
			NEXT_TK_CHECK_TOKEN(colon);

			NEXT_TK_CHECK_RULE(func_type);
			
			NEXT_TK_CHECK_TOKEN(left_curly_bracket);
			{
				NEXT_TK_CHECK_RULE(statement);
			}
			NEXT_TK_CHECK_TOKEN(right_curly_bracket);
			
			{ CODEGEN(emit_function_close, pd->rhs_func->id); }
			
			pd->in_local_scope = false;
			NEXT_TK_CHECK_RULE(program);
		}
		
		//<program> -> <statement>
		else
		{
			_DPRNR(1);
			CHECK_RULE(statement)
			//CHECK_RULE(program);
		}

	}
    RULE_CLOSE;
}

static int end(ParserData* pd)
{
	RULE_OPEN;
	{
		//<program> -> ?>
		if(TOKEN_IS(end))
		{
			_DPRNR(0);
			NEXT_TK_CHECK_TOKEN(EOF);
		}
		//<program> -> EOF
		else
		{
			_DPRNR(1);
			CHECK_TOKEN(EOF)
		}

		{ CODEGEN(emit_program_body_close); }
	}
    RULE_CLOSE;
}

static int begin(ParserData* pd)
{
	//<begin> -> <?php declare ( strict_types = 1 ) ; <program>
	RULE_OPEN;
	{
		{
			NEXT_TK_CHECK_TOKEN(prologue);
			_DPRNR(0);
			NEXT_TK_CHECK_TOKEN(ID);
			if (!str_cmp(pd->token.string, "declare"))
				PRINT_ERROR_RET(ERROR_SYNTAX, "invalid prologue.");
			
			NEXT_TK_CHECK_TOKEN(left_bracket);
			NEXT_TK_CHECK_TOKEN(ID);
			if (!str_cmp(pd->token.string, "strict_types"))
				PRINT_ERROR_RET(ERROR_SYNTAX, "invalid prologue.");
				
			NEXT_TK_CHECK_TOKEN(equal_sign);
			NEXT_TK_CHECK_TOKEN(integer);
			if (pd->token.integer != 1)
				PRINT_ERROR_RET(ERROR_SYNTAX, "invalid prologue.");

			NEXT_TK_CHECK_TOKEN(right_bracket);
			NEXT_TK_CHECK_TOKEN(semicolon);
		}

		{ CODEGEN(emit_program_body_open); }

		NEXT_TK_CHECK_RULE(program);
		NEXT_TK_CHECK_RULE(end);
	}
    RULE_CLOSE;
}

int parse_file(FILE* fptr)
{
	str_t string;
    ParserData pd;
    if (!str_const(&string) || !init_data(&pd) || !code_generator_init())
		goto error;

	scanner_set_file(fptr);
	scanner_set_string(&string);

    int result = begin(&pd);
	code_generator_flush(g_CodegenOut);
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
