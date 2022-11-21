#include <string.h>

#include "parser.h"
#include "errors.h"
#include "scanner.h"
#include "symtable.h"
#include "precedence_t.h"
#include "code_generator.h"

GENERATE_VECTOR_DEFINITION(Token, tk);

//bool g_LastTokenWasFromStack = false;

#include "macros.h"

#define TOKEN_PASS 0
#define FUNCTION_PASS 1
#define MAIN_PASS 2

static int s_TokenCntFuncPass = 0;

#define VEC_NEXT_TOKEN do {\
	INTERNAL(!tkvec_empty(&pd->tk_vec));\
	pd->token = pd->front_ptr->data;\
} while (0)

int _get_next_token(ParserData* pd)
{
	int RES = SUCCESS;
	if (!pd->block_next_token) 
	{
		static tkelem_t* front_ptr = NULL;
		switch (pd->mode)
		{
		case TOKEN_PASS:
			if ((RES = scanner_get_next_token(&pd->token)) != SUCCESS)
				return RES;
			PUSH_TOKEN_BACK;
			break;
		case FUNCTION_PASS:
			// if ((RES = scanner_get_next_token(&pd->token)) != SUCCESS)
			// 	return RES;
			if (!pd->front_ptr)
				pd->front_ptr = pd->tk_vec.front;
			
			if (!pd->func_pass_erase_tokens)
			{
				VEC_NEXT_TOKEN;
			}
			else
			{
				pd->token = tkvec_extract(&pd->tk_vec, pd->front_ptr);
			}

			pd->front_ptr = pd->front_ptr->prev;

			// if (pd->func_pass_erase_tokens)
			// 	++s_TokenCntFuncPass;
			break;
		case MAIN_PASS:
			if (!pd->front_ptr)
				pd->front_ptr = pd->tk_vec.front;
			VEC_NEXT_TOKEN;
			break;
		default:
			INTERNAL_ERROR_RET;
			break;
		}

		if (g_DebugOn) 
		{ 
			set_debug_out(get_scan_out());
			debug_print_token(pd->token); 
		}
	} 
	else
		pd->block_next_token = false;
	return SUCCESS;
}

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

static int function_pass(ParserData* pd);

static bool init_data(ParserData* pd)
{
    symtable_init(&pd->globalTable);
	//symtable_init(&pd->localTable);
	// pd->get_next_from_stack = 
	pd->mode = TOKEN_PASS;
	pd->front_ptr = NULL;

	pd->in_param_list   = pd->block_next_token = 
	pd->in_local_scope  = pd->return_found 	   =
	pd->var_not_yet_def = 
	pd->func_pass_erase_tokens = 
	pd->in_if_while 	= false;

	pd->rhs_func = pd->current_func = pd->lhs_var = NULL;

	pd->param_index = 0;
	pd->label_deep = -1;
	pd->label_index = 0;

	//str_const(&pd->var_name);
	//token_const(&pd->token);

	tkvec_init(&pd->tk_vec);
	//tkvec_init(&pd->tk_dispose_list);
	//strvec_init(&s_StringVect);

	TData* data = NULL;
	ADD_ID_TYPE(data, "reads", TYPE_STRING);
	ADD_ID_TYPE(data, "readi", TYPE_INT);
	ADD_ID_TYPE(data, "readf", TYPE_FLOAT);

	ADD_ID_TYPE(data, "write", TYPE_UNDEF);

	ADD_ID_TYPE(data, "floatval", TYPE_FLOAT);
	ADD_PARAM(data, TYPE_UNDEF, true);
	ADD_ID_TYPE(data, "intval", TYPE_INT);
	ADD_PARAM(data, TYPE_UNDEF, true);
	ADD_ID_TYPE(data, "strval", TYPE_STRING);
	ADD_PARAM(data, TYPE_UNDEF, true);

	ADD_ID_TYPE(data, "strlen", TYPE_INT);
	ADD_PARAM(data, TYPE_STRING, false);
	ADD_ID_TYPE(data, "substring", TYPE_STRING);
	ADD_PARAM(data, TYPE_STRING, false);
	ADD_PARAM(data, TYPE_INT, false);
	ADD_PARAM(data, TYPE_INT, false);

	ADD_ID_TYPE(data, "ord", TYPE_INT);
	ADD_PARAM(data, TYPE_STRING, false);
	ADD_ID_TYPE(data, "chr", TYPE_STRING);
	ADD_PARAM(data, TYPE_INT, false);

	ADD_ID_TYPE(data, "EXPR_VAL", TYPE_BOOL);

	return true;
}

static void free_data(ParserData* pd)
{
	symtable_dest(&pd->globalTable);
	//token_dest(&pd->token);

	//tkvec_dispose(&pd->tk_dispose_list, token_dest);
	tkvec_dispose(&pd->tk_vec, token_dest);
	IFJ22_ASSERT(tkvec_empty(&pd->tk_vec), "");
	//IFJ22_ASSERT(tkvec_empty(&pd->tk_dispose_list), "");
}

static int term(ParserData* pd)
{
	

	RULE_OPEN;
	{
		if (!strcmp(pd->rhs_func->id, "write")) // if function is "write"
		{
			if (IS_VAR_ID && !FIND_CURRENT_ID)
				PRINT_ERROR_RET(ERROR_SEM_UNDEF_VAR, "undefined variable passed as parameter.");

			// we need to store args to stack to pass them in inverse order
			//PUSH_TOKEN_FRONT;
			PUSH_TOKEN_BACK;

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
				//PUSH_TOKEN_BACK; //<-- save last token to then continue from where we stopped
				PUSH_TOKEN_FRONT;
				int i = 0;
				while (i < pd->param_index)
				{
					IFJ22_ASSERT(!tkvec_empty(&pd->tk_vec), "Shouldn't be empty.");
					//POP_TOKEN_FRONT;
					POP_TOKEN_BACK;

					{ CODEGEN(emit_function_pass_param_push, pd->token, pd->in_local_scope); }
					++i;
				}
				// We need to define argument count for variable number of arguments
				{ CODEGEN(emit_function_pass_param_count, pd->param_index); }
				POP_TOKEN_FRONT;
				//POP_TOKEN_BACK;
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
			if (pd->current_func)
				pd->current_func->qmark_type = true;
			//pd->func_questionmark = true;
			_rulenr += 3;
		}
		else
		{
			//pd->func_questionmark = false;
			if (pd->current_func)
				pd->current_func->qmark_type = false;
		}

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
				//If next token is not '(' then it's not a function call so we return syntax error
					//NEXT_TK_CHECK_TOKEN(left_bracket);
				//Check if function was defined
				if (!FIND_FUNC_ID)
				{
					const char* lhs_var_id  = NULL;
					const char* curr_func_id = NULL;
					const char* rhs_func_id  = NULL;
					if (pd->lhs_var)
						lhs_var_id = pd->lhs_var->id;
					if (pd->current_func)
						curr_func_id = pd->current_func->id;
					if (pd->rhs_func)
						rhs_func_id = pd->rhs_func->id;

					int prev_mode = pd->mode;
					Token last_token = pd->token;
					pd->mode = FUNCTION_PASS;
					pd->func_pass_erase_tokens = false;
					CHECK_RULE(function_pass);
					pd->func_pass_erase_tokens = true;
					pd->mode = prev_mode;
					pd->token = last_token;
					pd->front_ptr = pd->tk_vec.front;

					if (lhs_var_id)
						pd->lhs_var = FIND_ID(lhs_var_id);
					if (curr_func_id)
						pd->current_func = symtable_find(&pd->globalTable, curr_func_id);
					if (rhs_func_id)
						pd->rhs_func = symtable_find(&pd->globalTable, rhs_func_id);


					if (!FIND_FUNC_ID)
						PRINT_ERROR_RET(ERROR_SEM_ID_DEF, "undefined function.");
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
					PRINT_ERROR_RET(ERROR_SEM_ID_DEF, "parameter redefinition.");
				TData* data = NULL;
				ADD_CURRENT_ID(data); //pd->lhs_var
				int pnum = pd->current_func->params->len;
				int sign = pd->current_func->params->ptr[pnum-1];
				if (sign == 'i' || sign == LETTFLIP('i'))
					data->type = TYPE_INT;
				else if (sign == 'f' || sign == LETTFLIP('f'))
					data->type = TYPE_FLOAT;
				else if (sign == 's' || sign == LETTFLIP('s'))
					data->type = TYPE_STRING;
				else
					INTERNAL_ERROR_RET;
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
			// Token next_cpy;
			// token_cpy(&id_cpy, &pd->token);
			//PUSH_TOKEN_FRONT;

			//GET_NEXT_TOKEN;
			// token_cpy(&next_cpy, &pd->token);
			// pd->token = id_cpy;
			//PUSH_TOKEN_FRONT;
			//pd->token = next_cpy;
			Token next = pd->front_ptr->data;
			if (next.type != token_equal_sign)
			{
				//PUSH_TOKEN_FRONT;

				//token_dest(&pd->token);
				//pd->token = id_cpy;
				//POP_TOKEN_BACK;

				//pd->get_next_from_stack = true;
				goto statement_rvalue;
			}

			// token_cpy(&s_TokenBackup, &pd->token);
				
			// token_dest(&pd->token);
			// pd->token = id_cpy;
			// PUSH_TOKEN_FRONT;
			// POP_TOKEN_BACK;
			//pd->get_next_from_stack = true;

			{
				if (!(pd->lhs_var = FIND_CURRENT_ID))
				{
					ADD_CURRENT_ID(pd->lhs_var);
					{ CODEGEN(emit_define_var, pd->lhs_var->id, pd->in_local_scope); }
				}

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
			if (!(pd->lhs_var = symtable_find(&pd->globalTable, "EXPR_VAL")))
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
			pd->lhs_var = symtable_find(&pd->globalTable, "EXPR_VAL");
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
				if (!pd->in_local_scope) 
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
					
					if (!(pd->lhs_var = symtable_find(&pd->globalTable, "EXPR_VAL")))
						INTERNAL_ERROR_RET;
				}
			}
			CHECK_RULE(expression_parsing);

			if (pd->in_local_scope)
			{
				pd->return_found = true;

				{ CODEGEN(emit_call_return_sem_check); }
				{ CODEGEN(emit_function_return, pd->current_func->id); }
			}
			else
				{ CODEGEN(emit_clear_stack); }

			// if (pd->in_local_scope)
			// {
			// 	if (pd->lhs_var->type != pd->current_func->type)
			// 	{
			// 		if (!pd->func_questionmark || pd->lhs_var->type != TYPE_NULL)
			// 			PRINT_ERROR_RET(ERROR_SEM_TYPE_COMPAT, "invalid function return type.");
			// 	}
			// }

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

			if (pd->in_local_scope && pd->mode != FUNCTION_PASS)
				return ERROR_SYNTAX; //<-- function can't be defined inside another function
			
			NEXT_TK_CHECK_FUNC_ID;
			{ //Add func id to global table
				if (FIND_CURRENT_ID) //Function already defined
					PRINT_ERROR_RET(ERROR_SEM_ID_DEF, "function is already defined.");
				//ADD_CURRENT_ID(pd->current_func);
				ADD_FUNC_ID;
			}		

			bool was_in_local_before = pd->in_local_scope;
			pd->in_local_scope = true;
			TSymtable prev_table = pd->localTable;
			symtable_init(&pd->localTable);

			{ CODEGEN(emit_function_open, pd->current_func->id); }

			NEXT_TK_CHECK_TOKEN(left_bracket);
			{
				NEXT_TK_CHECK_RULE(params);
			}
			NEXT_TK_CHECK_TOKEN(right_bracket);
			NEXT_TK_CHECK_TOKEN(colon);
			
			NEXT_TK_CHECK_RULE(func_type);

			{ CODEGEN(emit_function_result_declaration, pd->current_func->type); }

			{ CODEGEN(emit_function_type, pd->current_func->type, pd->current_func->qmark_type); }
			
			pd->return_found = false;
			NEXT_TK_CHECK_TOKEN(left_curly_bracket);
			{
				NEXT_TK_CHECK_RULE(statement);
			}
			NEXT_TK_CHECK_TOKEN(right_curly_bracket);
			if (pd->current_func->type != TYPE_NULL && !pd->return_found)
				PRINT_ERROR_RET(ERROR_SEM_TYPE_COMPAT, 
					"function is not 'void' but 'return' is missing.");
			
			{ CODEGEN(emit_function_close, pd->current_func->id); }
			
			if (pd->mode != FUNCTION_PASS || !was_in_local_before)
				pd->in_local_scope = false;
			symtable_dest(&pd->localTable);
			pd->localTable = prev_table;
			if (pd->mode != FUNCTION_PASS)
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

static int function_pass(ParserData* pd)
{
	RULE_OPEN;
	{
		// for (int i = 0; i < s_TokenCntFuncPass; i++)
		// {
		// 	POP_TOKEN_FRONT; //get rid of all tokens read in begin rule
		// 	token_dest(&pd->token);
		// }

		do
		{
			
			Token next = pd->front_ptr->data;
			if (next.type == token_keyword && next.keyword == keyword_function)
			{
				//++s_TokenCntFuncPass; //<-- to also remove the function keyword
				pd->func_pass_erase_tokens = true;
				GET_NEXT_TOKEN;
				//GET_NEXT_TOKEN;
				CHECK_RULE(program);
				pd->func_pass_erase_tokens = false;
				break;
				// for (int i = 0; i < s_TokenCntFuncPass; i++)
				// {
				// 	POP_TOKEN_FRONT; //get rid of all tokens read in function rule
				// 	token_dest(&pd->token);
				// }
			}

			GET_NEXT_TOKEN;
			if (TOKEN_IS(end) || TOKEN_IS(EOF))
				break;
		} while (!TOKEN_IS(EOF));
	}
    RULE_CLOSE;
}

// static int token_pass(ParserData* pd)
// {
// 	while (!TOKEN_IS(EOF));
// 	{
// 		GET_NEXT_TOKEN;
// 	}
// }

static int begin(ParserData* pd)
{
	//<begin> -> <?php declare ( strict_types = 1 ) ; <program>
	RULE_OPEN;
	{
		pd->mode = TOKEN_PASS;
		//CHECK_RULE(token_pass);
		while (!TOKEN_IS(EOF))
		{
			GET_NEXT_TOKEN;
		}
		//pd->function_pass = true;
		pd->mode = FUNCTION_PASS;
		pd->front_ptr = pd->tk_vec.front;
		pd->func_pass_erase_tokens = true;
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

		pd->func_pass_erase_tokens = false;
		CHECK_RULE(function_pass);
		//pd->function_pass = false;
		pd->mode = MAIN_PASS;
		pd->func_pass_erase_tokens = false;
		pd->front_ptr = NULL;
		pd->front_ptr = pd->tk_vec.front;

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
