#include <string.h>

#include "parser.h"
#include "errors.h"
#include "scanner.h"
#include "symtable.h"
#include "precedence_t.h"
#include "code_generator.h"

GENERATE_VECTOR_DEFINITION(Token, tk);

// static tkvec_t s_TkVec;
// static tkvec_t s_TkDisposeList;

bool g_LastTokenWasFromStack = false;

//static Token s_TokenBackup;
#include "macros.h"

int _get_next_token(ParserData* pd)
{
	int RES = SUCCESS;
	if (!pd->block_next_token) 
	{
		if (pd->get_next_from_stack) // && !tkvec_empty(&s_TkVec)
		{
			POP_TOKEN_BACK;
			//pd->token = s_TokenBackup;

			pd->get_next_from_stack = false;
			
		}
		else 
		{
			if ((RES = scanner_get_next_token(&pd->token)) != SUCCESS)
				return RES;
			if (g_DebugOn) 
			{ 
				set_debug_out(get_scan_out());
				debug_print_token(pd->token); 
			}
			g_LastTokenWasFromStack = false;
		}
	} 
	else
		pd->block_next_token = false;
	return SUCCESS;
}



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if 1



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



#endif

static int type		(ParserData* pd);
static int term		(ParserData* pd);
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

static bool init_data(ParserData* pd)
{
    symtable_init(&pd->globalTable);
	//symtable_init(&pd->localTable);

	pd->in_param_list   = pd->block_next_token     = 
	pd->in_local_scope  = pd->func_questionmark    = 
	pd->var_not_yet_def = pd->get_next_from_stack  = 
	pd->in_if_while 	= false;

	pd->rhs_func = pd->current_func = pd->lhs_var = NULL;

	pd->param_index = 0;
	pd->label_deep = -1;
	pd->label_index = 0;

	//str_const(&pd->var_name);
	token_const(&pd->token);

	tkvec_init(&pd->tk_vec);
	tkvec_init(&pd->tk_dispose_list);
	//strvec_init(&s_StringVect);

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
	data->type = TYPE_INT;

	if (!(data = symtable_add_symbol(&pd->globalTable, "chr", &err)))
		return false;
	if (!symtable_add_param(data, TYPE_INT, false)) return false;
	data->type = TYPE_STRING;

	if (!(data = symtable_add_symbol(&pd->globalTable, "$EXPR_REG", &err)))
		return false;
	data->type = TYPE_BOOL;

	return true;
}

static void free_data(ParserData* pd)
{
	symtable_dest(&pd->globalTable);
	token_dest(&pd->token);

	tkvec_dispose(&pd->tk_dispose_list, token_dest);
	IFJ22_ASSERT(tkvec_empty(&pd->tk_vec), "");
	IFJ22_ASSERT(tkvec_empty(&pd->tk_dispose_list), "");
}

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
			PUSH_TOKEN_FRONT;

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

			if (!strcmp(pd->rhs_func->id, "write"))
			{
				//Token last_token = pd->token; //<-- save last token to then continue from where we stopped
				PUSH_TOKEN_BACK; //<-- save last token to then continue from where we stopped
				int i = 0;
				while (!tkvec_empty(&pd->tk_vec) && i < pd->param_index)
				{
					POP_TOKEN_FRONT;

					{ CODEGEN(emit_function_pass_param_push, pd->token, pd->in_local_scope); }
					++i;
				}
				// We need to define argument count for variable number of arguments
				{ CODEGEN(emit_function_pass_param_count, pd->param_index); }
				POP_TOKEN_BACK;
				//pd->token = last_token;
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
		//GET_NEXT_TOKEN;

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
			if (!symtable_add_param(pd->current_func, type_macro[tnum], pd->token.questionmark))
				INTERNAL_ERROR_RET;
		}
		else // If it's the return type of the current func.
			pd->current_func->type = type_macro[tnum];
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

			pd->rhs_func = NULL;
		}
		//<rvalue> -> <expression>
		else
		{
			_DPRNR(1);

			CHECK_RULE(expression_parsing);
		}
	}
	RULE_CLOSE;
}

static int func_type(ParserData* pd)
{
	RULE_OPEN;
	{
		//<func_type> -> void
		if (KEYWORD_IS(void))
		{
			_DPRNR(0);

			pd->current_func->type = TYPE_NULL;
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
		pd->in_param_list = true; // <-- referenced in 'type' rule

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

		pd->in_param_list = false; 
	}
	RULE_CLOSE;
}

static int statement(ParserData* pd)
{
	RULE_OPEN;
	{
		pd->lhs_var = NULL;
		// if (!pd->in_local_scope)
		// 	pd->rhs_func = NULL;

		//<statement> -> $ID = <rvalue> ; <program>
		if (IS_VAR_ID)
		{
			_DPRNR(0);

			// Token id_cpy;
			// token_cpy(&id_cpy, &pd->token);
			PUSH_TOKEN_FRONT;

			GET_NEXT_TOKEN;
			if (!TOKEN_IS(equal_sign))
			{
				PUSH_TOKEN_FRONT;
				//token_cpy(&s_TokenBackup, &pd->token);

				// token_dest(&pd->token);
				// pd->token = id_cpy;
				POP_TOKEN_BACK;

				pd->get_next_from_stack = true;
				goto statement_rvalue;
			}

			// token_cpy(&s_TokenBackup, &pd->token);
				
			// token_dest(&pd->token);
			// pd->token = id_cpy;
			PUSH_TOKEN_FRONT;
			POP_TOKEN_BACK;
			pd->get_next_from_stack = true;

			{
				if (!(pd->lhs_var = FIND_CURRENT_ID))
					ADD_CURRENT_ID(pd->lhs_var);

				{ CODEGEN(emit_define_var, pd->lhs_var->id, pd->in_local_scope); }

				NEXT_TK_CHECK_TOKEN(equal_sign);

				pd->var_not_yet_def = true;

				NEXT_TK_CHECK_RULE(rvalue);

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
			if (!(pd->lhs_var = symtable_find(&pd->globalTable, "$EXPR_REG")))
				INTERNAL_ERROR_RET;

			pd->label_deep++;
			const char* func_name = pd->current_func ? pd->current_func->id : "";
			{ CODEGEN(emit_if_head); }

			pd->in_if_while = true;
			CHECK_RULE(expression_parsing);
			pd->in_if_while = false;

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
			const char* func_name = pd->current_func ? pd->current_func->id : "";
			{ CODEGEN(emit_while_head, func_name, pd->label_deep, pd->label_index); }

			pd->in_if_while = true;
			CHECK_RULE(expression_parsing);
			pd->in_if_while = false;

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

			GET_NEXT_TOKEN;
			{
				if (!pd->in_local_scope) //<-- if main doesn't return anything
				{
					if (TOKEN_IS(semicolon))
						goto no_retcode;
				}
				else
				{
					if (pd->current_func->type == TYPE_NULL)
					{
						if (TOKEN_IS(semicolon))
							goto no_retcode;
						else
							PRINT_ERROR_RET(ERROR_SEM_RETURN, "invalid function return type (function is 'void').");

					}
					else if (TOKEN_IS(semicolon))
						PRINT_ERROR_RET(ERROR_SEM_RETURN, "invalid function return type (return value is missing).");
					
					if (!(pd->lhs_var = symtable_find(&pd->globalTable, "$EXPR_REG")))
						INTERNAL_ERROR_RET;
				}
			}
			CHECK_RULE(expression_parsing);

			if (pd->in_local_scope)
			{
				if (pd->lhs_var->type != pd->current_func->type)
				{
					if (!pd->func_questionmark || pd->lhs_var->type != TYPE_NULL)
						PRINT_ERROR_RET(ERROR_SEM_TYPE_COMPAT, "invalid function return type.");
				}
			}

			{ CODEGEN(emit_clear_stack); } //<-- get rid of return value on stack

			NEXT_TK_CHECK_TOKEN(semicolon);
no_retcode:
			NEXT_TK_CHECK_RULE(program);
		}
		//<statement> -> <rvalue> ; <program>
		else if (TOKEN_IS_VALUE_TYPE)
		{
statement_rvalue:
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
				ADD_CURRENT_ID(pd->current_func);
			}		

			pd->in_local_scope = true;
			symtable_init(&pd->localTable);

			{ CODEGEN(emit_function_open, pd->current_func->id); }

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
			
			{ CODEGEN(emit_function_close, pd->current_func->id); }
			
			pd->in_local_scope = false;
			symtable_dest(&pd->localTable);
			
			NEXT_TK_CHECK_RULE(program);
		}
		
		//<program> -> <statement>
		else
		{
			_DPRNR(1);
			CHECK_RULE(statement);
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
			if (!str_cmp(&pd->token.string, "declare"))
				PRINT_ERROR_RET(ERROR_SYNTAX, "invalid prologue.");
			
			NEXT_TK_CHECK_TOKEN(left_bracket);
			NEXT_TK_CHECK_TOKEN(ID);
			if (!str_cmp(&pd->token.string, "strict_types"))
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
	//str_t string;
    ParserData pd;
    if (!init_data(&pd) || !code_generator_init()) //!str_const(&string) || 
		goto error;

	scanner_set_file(fptr);

    int result = begin(&pd);
	code_generator_flush(g_CodegenOut);
	code_generator_terminate();

	goto free;

error:
	result = ERROR_INTERNAL;
free:
	free_data(&pd);
end:
    return result;
}
