#include <string.h>

#include "parser.h"
#include "errors.h"
#include "scanner.h"
#include "symtable.h"
#include "expression.h"
#include "codegen.h"
#include "macros.h"

GENERATE_VECTOR_DEFINITION(Token, tk);

int64_t parser_get_token(ParserData* pd)
{
	RULE_OPEN;
	{
		if (!pd->block_next_token) 
		{
			switch (pd->mode)
			{
			case MODE_FUNCTION_PASS:
				if ((RES = scanner_get_next_token(&pd->token)) != SUCCESS)
					return RES;
				PUSH_TOKEN_BACK;
				break;
			case MODE_MAIN_PASS:
				VILE_ASSERT(pd->front_ptr, "");
				VEC_NEXT_TOKEN;
				break;
			default:
				INTERNAL_ERROR_RET("Unsupported mode.");
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
	}
	RULE_CLOSE;
}

/////////////////////////////////////////////////////////////////////////

static int64_t type		(ParserData* pd);
static int64_t term		(ParserData* pd);
static int64_t rvalue	(ParserData* pd);
static int64_t statement(ParserData* pd);
static int64_t func_type(ParserData* pd);
static int64_t args		(ParserData* pd);
static int64_t arg_n	(ParserData* pd);
static int64_t params	(ParserData* pd);
static int64_t param_n	(ParserData* pd);
static int64_t program	(ParserData* pd);
static int64_t end		(ParserData* pd);
static int64_t begin	(ParserData* pd);

static int64_t function_pass   (ParserData* pd);
static int64_t conditional_pass(ParserData* pd);

/////////////////////////////////////////////////////////////////////////

static bool init_data(ParserData* pd)
{
    symtable_init(&pd->globalTable);

	pd->mode = MODE_MAIN_PASS;
	pd->front_ptr = NULL;

	pd->in_param_list   = pd->block_next_token = 
	pd->in_local_scope  = pd->return_found 	   =
	pd->var_not_yet_def = pd->in_if_while 	   = false;

	pd->rhs_func = pd->current_func = pd->lhs_var = NULL;

	pd->param_index = 0;

	pd->label_index = 0;
	pd->label_deep = -1;

	tkvec_init(&pd->tk_vec);

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
	tkvec_dispose(&pd->tk_vec, token_dest);
	VILE_ASSERT(tkvec_empty(&pd->tk_vec), "");
}

static int64_t function_pass(ParserData* pd)
{
	RULE_OPEN;
	{
		do {
			GET_NEXT_TOKEN;

			if (!KEYWORD_IS(function))
				continue;

			NEXT_TK_CHECK_FUNC_ID;
			{ //Add func id to global table
				if (FIND_CURRENT_ID) //Function already defined
					PRINT_ERROR_RET(ERROR_SEM_ID_DEF, "function is already defined.");
				ADD_FUNC_ID;
			}		

			pd->in_local_scope = true;
			symtable_init(&pd->localTable);

			NEXT_TK_CHECK_TOKEN(left_bracket);
			{
				NEXT_TK_CHECK_RULE(params);
			}
			NEXT_TK_CHECK_TOKEN(right_bracket);
			NEXT_TK_CHECK_TOKEN(colon);
			
			NEXT_TK_CHECK_RULE(func_type);

			pd->in_local_scope = false;
			symtable_dest(&pd->localTable);
		} 
		while (!TOKEN_IS(EOF)); //!TOKEN_IS(end) && 
	}
    RULE_CLOSE;
}

static int64_t conditional_pass(ParserData* pd)
{
	RULE_OPEN;
	{
		Token next;
		int64_t rcbr_count = 0;
		int64_t rcbr_need = 1;
		if (KEYWORD_IS(if))
			++rcbr_need;

		while (rcbr_count < rcbr_need && !TOKEN_IS(EOF))
		{
			GET_NEXT_TOKEN;

			VILE_ASSERT(pd->front_ptr, "front_ptr null.");
			next = pd->front_ptr->data;
			if (IS_VAR_ID && next.type == token_equal_sign)
			{
				if (!(pd->lhs_var = FIND_CURRENT_ID))
				{
					ADD_CURRENT_ID(pd->lhs_var);
					{ CODEGEN(emit_define_var, pd->lhs_var->id, pd->in_local_scope); }
				}
			}

			if (TOKEN_IS(right_curly_bracket))
				++rcbr_count;

			if (KEYWORD_IS(if))
				rcbr_need += 2;
			else if KEYWORD_IS(while)
				++rcbr_need;
		}
	}
	RULE_CLOSE;
}

/////////////////////////////////////////////////////////////////////////

static int64_t term(ParserData* pd)
{
	RULE_OPEN;
	{
		if (TOKEN_IS(ID))
		 	{ CODEGEN(emit_check_var_defined, TK_STR(pd->token), pd->in_local_scope); }

		if (!strcmp(pd->rhs_func->id, "write")) // if function is "write"
		{
			if (IS_VAR_ID && !FIND_CURRENT_ID)
				PRINT_ERROR_RET(ERROR_SEM_UNDEF_VAR, "undefined variable passed as parameter.");

			// we need to store args to stack to pass them in inverse order
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

		int64_t func_param = pd->rhs_func->params->ptr[pd->param_index];
		int64_t sign = '\0';
		TData* var = NULL;
		switch (pd->token.type)
		{
			//<term> -> INT_VAL
			case token_integer:
				_DPRNR(0);
				sign = 'i';	
				break;
			//<term> -> FLOAT_VAL
			case token_float:
				_DPRNR(1);
				sign = 'f';	
				break;
			//<term> -> STRING_VAL
			case token_string:
				_DPRNR(2);
				sign = 's';	
				break;
			//<term> -> null
			case token_null:
				_DPRNR(3);
				goto null;
				break;
			//<term> -> $ID
			case token_ID:
				_DPRNR(4);

				var = FIND_CURRENT_ID;
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
						INTERNAL_ERROR_RET("Unsupported.");
				}
				break;
			default:
				INTERNAL_ERROR_RET("Unsupported.");
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

static int64_t args(ParserData* pd)
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
				PUSH_TOKEN_FRONT; //Save last token to then continue from where we stopped.
				int64_t i = 0;
				while (i < pd->param_index)
				{
					VILE_ASSERT(!tkvec_empty(&pd->tk_vec), "Tk_vec empty.");
					POP_TOKEN_BACK;
					
					{ CODEGEN(emit_function_pass_param_push, pd->token, pd->in_local_scope); }
					++i;
				}
				
				//We need to define argument count for variable number of arguments.
				{ CODEGEN(emit_function_pass_param_count, pd->param_index); }
				POP_TOKEN_FRONT; //Continue with the last token
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

static int64_t arg_n(ParserData* pd)
{
	RULE_OPEN;
	{
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

static int64_t type(ParserData* pd)
{
	#define NUM_F_TYPES 3
	RULE_OPEN;
	{
		/* <type> -> float // <type> -> int // <type> -> string */

		static const int64_t type_macro[NUM_F_TYPES] = { TYPE_FLOAT, TYPE_INT, TYPE_STRING };
		
		int64_t _rulenr = 0; // for debugging

		if (pd->token.questionmark)
		{
			//Means it can be null!
			if (pd->current_func)
				pd->current_func->qmark_type = true;
			_rulenr += 3;
		}
		else
		{
			if (pd->current_func)
				pd->current_func->qmark_type = false;
		}

		int64_t  tnum = 0;
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

		if (pd->in_param_list && pd->mode == MODE_FUNCTION_PASS) // If it's the type for func. parameter
		{
			if (!symtable_add_param(pd->current_func, type_macro[tnum], pd->token.questionmark))
				INTERNAL_ERROR_RET("Internal error.");
		}
		else // If it's the return type of the current func.
			pd->current_func->type = type_macro[tnum];
	}
	RULE_CLOSE;
}

static int64_t rvalue(ParserData* pd)
{
	RULE_OPEN;
	{
		//<rvalue> -> ID ( <args> )
		if (IS_FUNC_ID)
		{
			_DPRNR(0);

			{ 
				Token next;
				if (pd->front_ptr)
					next = pd->front_ptr->data;
				if (next.type != token_left_bracket) //If next token is not '(' then it's not a function call,
					return ERROR_SYNTAX;			 //so we return syntax error.
				
				if (!(pd->rhs_func = FIND_FUNC_ID)) //Check if function was defined.
					PRINT_ERROR_RET(ERROR_SEM_ID_DEF, "Undefined function.");

				if (pd->lhs_var) //If function is called as part of variable definition.
						pd->lhs_var->type = pd->rhs_func->type;

				{ CODEGEN(emit_function_before_params); }

				NEXT_TK_CHECK_TOKEN(left_bracket);
				{
					NEXT_TK_CHECK_RULE(args);
				}
				NEXT_TK_CHECK_TOKEN(right_bracket);

				if (strcmp(pd->rhs_func->id, "write") && 
					pd->rhs_func->params->len != pd->param_index)
					return ERROR_SEM_TYPE_COMPAT;
				
				{ CODEGEN(emit_function_call, pd->rhs_func->id); }
				
				//If function is called as part of variable definition.
				if (pd->lhs_var)
					{ CODEGEN(emit_function_result_assign, pd->lhs_var->id, pd->in_local_scope); }
			}

			pd->rhs_func = NULL;
		}
		//<rvalue> -> <expression>
		else
		{
			_DPRNR(1);

			CHECK_RULE(expression);
		}
	}
	RULE_CLOSE;
}

static int64_t func_type(ParserData* pd)
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

static int64_t param_type(ParserData* pd)
{
	RULE_OPEN;
	{
		NEXT_TK_CHECK_VAR_ID;

		if (FIND_CURRENT_ID)
			PRINT_ERROR_RET(ERROR_SEM_ID_DEF, "Parameter already defined.");
		TData* data = NULL;
		ADD_CURRENT_ID(data);
		int64_t sign = pd->current_func->params->ptr[pd->param_index];

		if (sign == 'i' || sign == LETTFLIP('i'))
			data->type = TYPE_INT;
		else if (sign == 'f' || sign == LETTFLIP('f'))
			data->type = TYPE_FLOAT;
		else if (sign == 's' || sign == LETTFLIP('s'))
			data->type = TYPE_STRING;
		else
			INTERNAL_ERROR_RET();

		{ CODEGEN(emit_function_param_declare, TK_STR(pd->token), pd->param_index); }
		pd->param_index++;
	}
	RULE_CLOSE;
}

static int64_t param_n(ParserData* pd)
{
	RULE_OPEN;
	{
		//<param_n> -> , <type> $ ID <param_n>
		if (TOKEN_IS(comma))
		{
			_DPRNR(0);

			NEXT_TK_CHECK_RULE(type);

			CHECK_RULE(param_type);

			NEXT_TK_CHECK_RULE(param_n);
		}
		//<param_n> -> ε
		else
		{
			_DPRNR(0);
			BLOCK_NEXT_TOKEN;
		}
	}
	RULE_CLOSE;
}

static int64_t params(ParserData* pd)
{
	RULE_OPEN;
	{
		pd->in_param_list = true; //referenced in 'type' rule.
		pd->param_index = 0;

		//<params> -> <type> $ ID <param_n>
		if (TOKEN_IS_TYPE_KEYWORD)
		{
			_DPRNR(0);

			CHECK_RULE(type);

			CHECK_RULE(param_type);

			NEXT_TK_CHECK_RULE(param_n);
		}
		//<params> -> ε
		else
		{
			_DPRNR(1);
			BLOCK_NEXT_TOKEN;
		}

		pd->in_param_list = false; 
	}
	RULE_CLOSE;
}

static int64_t statement(ParserData* pd)
{
	RULE_OPEN;
	{
		pd->lhs_var = NULL;
		Token next;
		if (pd->front_ptr)
			next = pd->front_ptr->data;
		//<statement> -> $ID = <rvalue> ; <program>
		if (IS_VAR_ID && next.type == token_equal_sign)
		{
			_DPRNR(0);

			{
				if (!pd->in_if_while)
				{
					if (!(pd->lhs_var = FIND_CURRENT_ID))
					{
						ADD_CURRENT_ID(pd->lhs_var);
						{ CODEGEN(emit_define_var, pd->lhs_var->id, pd->in_local_scope); }
					}
				}
				else 
					pd->lhs_var = FIND_CURRENT_ID;

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

			pd->in_if_while = true;

			tkelem_t* main_front = pd->front_ptr;
			CHECK_RULE(conditional_pass);
			pd->front_ptr = main_front;

			NEXT_TK_CHECK_TOKEN(left_bracket);

			if (!(pd->lhs_var = symtable_find(&pd->globalTable, "EXPR_VAL")))
				INTERNAL_ERROR_RET();

			pd->label_deep++;
			int64_t curr_lb_index = pd->label_index;
			pd->label_index += 2;
			const char* func_name = pd->current_func ? pd->current_func->id : "";
			{ CODEGEN(emit_if_header); }

			{
				CHECK_RULE(expression);
				{ CODEGEN(emit_expression_bool_convert); }
			}
			
			{ CODEGEN(emit_if_open, func_name, pd->label_deep, curr_lb_index); }

			NEXT_TK_CHECK_TOKEN(left_curly_bracket);
			{
				NEXT_TK_CHECK_RULE(statement);
			}
			NEXT_TK_CHECK_TOKEN(right_curly_bracket);

			NEXT_TK_CHECK_KEYWORD(else);

			{ CODEGEN(emit_else, func_name, pd->label_deep, curr_lb_index); }

			NEXT_TK_CHECK_TOKEN(left_curly_bracket);
			{
				NEXT_TK_CHECK_RULE(statement);
			}
			NEXT_TK_CHECK_TOKEN(right_curly_bracket);

			{ CODEGEN(emit_if_close, func_name, pd->label_deep, curr_lb_index + 1); }

			pd->label_deep--;
			pd->in_if_while = false;

			NEXT_TK_CHECK_RULE(program);
		}
		//<statement> -> while ( <expression> ) { <statement> } <program>
		else if (KEYWORD_IS(while))
		{
			_DPRNR(2);

			pd->in_if_while = true;

			tkelem_t* main_front = pd->front_ptr;
			CHECK_RULE(conditional_pass);
			pd->front_ptr = main_front;

			NEXT_TK_CHECK_TOKEN(left_bracket);
			pd->lhs_var = symtable_find(&pd->globalTable, "EXPR_VAL");
			if (!pd->lhs_var)
				INTERNAL_ERROR_RET();

			pd->label_deep++;
			int64_t curr_lb_index = pd->label_index;
			pd->label_index += 2;
			const char* func_name = pd->current_func ? pd->current_func->id : "";
			{ CODEGEN(emit_while_header, func_name, pd->label_deep, curr_lb_index); }

			{
				CHECK_RULE(expression);
				{ CODEGEN(emit_expression_bool_convert); }
			}
			
			{ CODEGEN(emit_while_open, func_name, pd->label_deep, curr_lb_index + 1); }

			NEXT_TK_CHECK_TOKEN(left_curly_bracket);
			{
				NEXT_TK_CHECK_RULE(statement);
			}
			NEXT_TK_CHECK_TOKEN(right_curly_bracket);

			{ CODEGEN(emit_while_close, func_name, pd->label_deep, curr_lb_index + 1); }

			pd->label_deep--;
			pd->in_if_while = false;

			NEXT_TK_CHECK_RULE(program);
		}
		//<statement> -> return <expression> ; <program>
		else if (KEYWORD_IS(return))
		{
			_DPRNR(3);

			pd->return_found = true;
			GET_NEXT_TOKEN;
			bool no_retcode = false;
			
			if (pd->in_local_scope)
			{
				bool is_void = pd->current_func->type == TYPE_NULL;
				if (is_void)
				{
					if (TOKEN_IS(semicolon))
					{
						no_retcode = true;
						BLOCK_NEXT_TOKEN;
					}
					else
						PRINT_ERROR_RET(ERROR_SEM_RETURN, "invalid function return type (function is 'void').");

				}
				else if (TOKEN_IS(semicolon))
					PRINT_ERROR_RET(ERROR_SEM_RETURN, "invalid function return type (return value is missing).");
				
				if (!(pd->lhs_var = symtable_find(&pd->globalTable, "EXPR_VAL")))
					INTERNAL_ERROR_RET();
				
				if (!no_retcode)
				{
					CHECK_RULE(expression);

					{ CODEGEN(emit_call_return_sem_check); }
				}
				
				{ CODEGEN(emit_function_return_statement, pd->current_func->id, is_void); }
			}
			else
			{	
				if (TOKEN_IS(semicolon))
				{
					no_retcode = true;
					BLOCK_NEXT_TOKEN;
				}

				if (!no_retcode)
					CHECK_RULE(expression);
				
				{ CODEGEN(emit_clear_stack); }
			}
			
			NEXT_TK_CHECK_TOKEN(semicolon);

			NEXT_TK_CHECK_RULE(program);
		}
		//<statement> -> <rvalue> ; <program>
		else if (TOKEN_IS_VALUE_TYPE)
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

static int64_t program(ParserData* pd)
{
	RULE_OPEN;
	{
		//<program> -> function ID ( <params> ) : <func_type> { <statement> } <program>
		if (KEYWORD_IS(function))
		{
			_DPRNR(0);

			if (pd->in_local_scope)
				PRINT_ERROR_RET(ERROR_SYNTAX, "function definition in local scope.");
			
			NEXT_TK_CHECK_FUNC_ID;
			{
				if (!(pd->current_func = FIND_CURRENT_ID)) // Should've been added in function pass
				 	PRINT_ERROR_RET(ERROR_SEM_ID_DEF, "undefined function.");
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

			{ CODEGEN(emit_function_result_declaration, pd->current_func->type); }
			{ CODEGEN(emit_function_type, pd->current_func->type, pd->current_func->qmark_type); }
			
			pd->return_found = false;
			NEXT_TK_CHECK_TOKEN(left_curly_bracket);
			{
				NEXT_TK_CHECK_RULE(statement);
			}
			NEXT_TK_CHECK_TOKEN(right_curly_bracket);
			if (pd->in_local_scope && pd->current_func->type != TYPE_NULL 
				&& !pd->return_found)
				{ PRINT_ERROR_RET(ERROR_SEM_TYPE_COMPAT, "function is not 'void' but 'return' is missing."); }
			
			{ CODEGEN(emit_function_close, pd->current_func->id); }
			
			pd->in_local_scope = false;
			pd->current_func = NULL;
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

static int64_t end(ParserData* pd)
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

static int64_t begin(ParserData* pd)
{
	//<begin> -> <?php declare ( strict_types = 1 ) ; <program> <end>
	RULE_OPEN;
	{
		pd->mode = MODE_FUNCTION_PASS;
		CHECK_RULE(function_pass);
		pd->mode = MODE_MAIN_PASS;
		pd->front_ptr = pd->tk_vec.front;

		{ //Check prologue sequence.
			_DPRNR(0);
			NEXT_TK_CHECK_TOKEN(prologue);
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

int64_t parse_file(FILE* fptr)
{
    ParserData pd;
    if (!init_data(&pd) || !code_generator_init())
		goto error;

	scanner_set_file(fptr);

    int64_t result = begin(&pd);

	code_generator_terminate();
	goto free;

error:
	result = ERROR_INTERNAL;
free:
	free_data(&pd);
    return result;
}
