/**
 * Project: Implementation of imperative language IFJ22 compiler.
 * 
 * @author Vadim Goncearenco (xgonce00@stud.fit.vutbr.cz)
 */

#ifndef __MACROS_H__
#define __MACROS_H__

#include "debug.h"

/* HELPER MACROS FOR USE IN DIFFERENT PARTS OF COMPILER */

#define INTERNAL(_expr, ...) if (!(_expr)) { INTERNAL_ERROR_RET(__VA_ARGS__); } else {}
#define SYNTAX(_expr) 		 if (!(_expr)) { return ERROR_SYNTAX;  			  } else {}

#if 1 /* Macros for 'vector_t' methods */
	#define PUSH_TOKEN_FRONT do {\
			INTERNAL(tkvec_push_front(&pd->tk_vec, pd->token));\
		} while(0)

	#define POP_TOKEN_FRONT do {\
			INTERNAL(!tkvec_empty(&pd->tk_vec));\
			pd->token = tkvec_pop_front(&pd->tk_vec);\
		} while(0)

	#define PUSH_TOKEN_BACK do {\
			INTERNAL(tkvec_push_back(&pd->tk_vec, pd->token));\
		} while(0)

	#define POP_TOKEN_BACK  do {\
			INTERNAL(!tkvec_empty(&pd->tk_vec));\
			pd->token = tkvec_pop_back(&pd->tk_vec);\
		} while(0)
#endif

#if 1 /* Helper macros for getting the next token from scanner */
	#define GET_NEXT_TOKEN do {\
			if ((RES = parser_get_token(pd)) != SUCCESS)\
				return RES;\
		} while(0)

	#define VEC_NEXT_TOKEN do {\
			INTERNAL(!tkvec_empty(&pd->tk_vec));\
			pd->token = pd->front_ptr->data;\
			pd->front_ptr = pd->front_ptr->next;\
		} while (0)

	/* Macro used in epsilon rules to avoid reading next token */
	#define BLOCK_NEXT_TOKEN pd->block_next_token = true
#endif

/////////////////////////////////////////////////////////////////////////

#if 1 /* Macros ensuring presence of return code variable in grammar rules */
	#define RES result
	#define DEF_RES int64_t RES = SUCCESS;
	#define RULE_OPEN  DEF_RES 
	#define RULE_CLOSE return RES
#endif

/////////////////////////////////////////////////////////////////////////

/* Parser work modes */
#define MODE_MAIN_PASS 	   0
#define MODE_FUNCTION_PASS 1

/* Helper macro to access string pointer of current token */
#define TK_STR(_tk) _tk.string.ptr 

/////////////////////////////////////////////////////////////////////////

#if 1 /* Helper macros for checking attributes of current token */
	#define TOKEN_IS(_postfix) (pd->token.type == token_##_postfix					   )
	#define KEYWORD_IS(_kw)    (TOKEN_IS(keyword) && pd->token.keyword == keyword_##_kw)

	#if 1 /* Never use these macros with more than 1 argument! 
			 A concatenation trick is used that allows passing C language keywords to macros. 
			 E.g. : 'if', 'else', 'EOF' etc. */
		#define VALIDATE_TOKEN(_postfix, ...) SYNTAX(TOKEN_IS(_postfix##__VA_ARGS__))
		#define VALIDATE_KEYWORD(_kw, ...)    SYNTAX(KEYWORD_IS(_kw##__VA_ARGS__)	 )

		#define NEXT_TK_VALIDATE_TOKEN(_type, ...) do {\
				GET_NEXT_TOKEN;\
				VALIDATE_TOKEN(_type##__VA_ARGS__);\
			} while(0)

		#define NEXT_TK_VALIDATE_KEYWORD(_kw, ...) do {\
				GET_NEXT_TOKEN;\
				VALIDATE_KEYWORD(_kw##__VA_ARGS__);\
			} while(0)
	#endif
#endif

/////////////////////////////////////////////////////////////////////////

#if 1 /* Helper macros for grammar rule return code propagation */
	#define VALIDATE_RULE(_rule)\
		if ((RES = _rule(pd))) { return RES; } else {}

	#define NEXT_TK_VALIDATE_RULE(_rule) do {\
			GET_NEXT_TOKEN;\
			VALIDATE_RULE(_rule);\
		} while(0)
#endif

/////////////////////////////////////////////////////////////////////////

#if 1 /* Helper macros for checking if token is an ID */
	#define HAS_DOLLAR (TK_STR(pd->token)[0] == '$')

	#define IS_VAR_ID  (TOKEN_IS(ID) && HAS_DOLLAR )
	#define IS_FUNC_ID (TOKEN_IS(ID) && !HAS_DOLLAR)

	#define NEXT_TK_VALIDATE_VAR_ID do {\
			NEXT_TK_VALIDATE_TOKEN(ID);\
			SYNTAX(HAS_DOLLAR);\
		} while(0)

	#define NEXT_TK_VALIDATE_FUNC_ID do {\
			NEXT_TK_VALIDATE_TOKEN(ID);\
			SYNTAX(!HAS_DOLLAR);\
		} while(0)
#endif

/////////////////////////////////////////////////////////////////////////

#if 1 /* Helper macros to access symtable */
	#define FIND_CURRENT_ID symtable_find(pd->in_local_scope ? &pd->localTable : &pd->globalTable, TK_STR(pd->token))
	#define FIND_FUNC_ID 	symtable_find(&pd->globalTable										 , TK_STR(pd->token))

	#define ADD_ID(_dst, _id, _table) do {\
			bool _err = false;\
			_dst = symtable_add_symbol(_table, _id, &_err);\
			if (_err || !_dst)\
				INTERNAL_ERROR_RET("function with this name was already added. OR memory error.");\
			_dst->global = !pd->in_local_scope;\
		} while(0)
	
	#define ADD_CURRENT_ID(_dst) ADD_ID(_dst		    , TK_STR(pd->token), pd->in_local_scope ? &pd->localTable : &pd->globalTable)
	#define ADD_FUNC_ID 		 ADD_ID(pd->current_func, TK_STR(pd->token), &pd->globalTable)

	#define ADD_ID_TYPE(_dst, _id, _type) do {\
			ADD_ID(_dst, _id, pd->in_local_scope ? &pd->localTable : &pd->globalTable);\
			_dst->type = _type;\
		} while(0)

	#define ADD_PARAM(_func, _type, _qmark)\
		if (!symtable_add_param(_func, _type, _qmark)) return false;
#endif

/////////////////////////////////////////////////////////////////////////

/* Helper macros to check type to determine if execution of certain rules should happen */
#define TOKEN_IS_TYPE_KEYWORD (KEYWORD_IS(float) || KEYWORD_IS(int)   || KEYWORD_IS(string)								     )
#define TOKEN_IS_VALUE_TYPE   (TOKEN_IS(float) 	 || TOKEN_IS(integer) || TOKEN_IS(string)   || TOKEN_IS(null) || TOKEN_IS(ID))

/////////////////////////////////////////////////////////////////////////

/* Flip the letter alphabet index to encode ?int, ?float, ?string types */
#define LETTFLIP(_s) ('z' - (_s) + 'a')

/////////////////////////////////////////////////////////////////////////

#ifdef VILE_DEBUG /* Macro for debug print of triggered grammar rule */
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
#else
	#define _DPRNR(_n)
#endif

#endif //  __MACROS_H__