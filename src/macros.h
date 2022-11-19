#ifndef __MACROS_H__
#define __MACROS_H__

#define INTERNAL(_expr) if (!(_expr)) { INTERNAL_ERROR_RET; } else {}

#define TOKEN_IS(_postfix) (pd->token.type == token_##_postfix)
#define TK_STR(_tk) _tk.string.ptr

#define PUSH_TOKEN_FRONT do {\
		Token _copy;\
		token_cpy(&_copy, &pd->token);\
		INTERNAL(tkvec_push_front(&pd->tk_vec, _copy));\
	} while(0)

#define POP_TOKEN_FRONT do {\
		INTERNAL(!tkvec_empty(&pd->tk_vec));\
		if (!g_LastTokenWasFromStack)\
			token_dest(&pd->token);\
		pd->token = tkvec_pop_front(&pd->tk_vec);\
		tkvec_push_back(&pd->tk_dispose_list, pd->token);\
		g_LastTokenWasFromStack = true;\
	} while(0)


#define PUSH_TOKEN_BACK do {\
		Token _copy;\
		token_cpy(&_copy, &pd->token);\
		INTERNAL(tkvec_push_back(&pd->tk_vec, _copy));\
	} while(0)

#define POP_TOKEN_BACK do {\
		INTERNAL(!tkvec_empty(&pd->tk_vec));\
		if (!g_LastTokenWasFromStack)\
			token_dest(&pd->token);\
		pd->token = tkvec_pop_back(&pd->tk_vec);\
		tkvec_push_back(&pd->tk_dispose_list, pd->token);\
		g_LastTokenWasFromStack = true;\
	} while(0)



#define GET_NEXT_TOKEN\
		do {\
			if ((RES = _get_next_token(pd)) != SUCCESS)\
				return RES;\
		} while(0)

#define RES result
#define DEF_RES int64_t RES = SUCCESS;
#define RULE_OPEN  DEF_RES 
#define RULE_CLOSE return RES

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
#else
	#define _DPRNR(_n)
#endif



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
		GET_NEXT_TOKEN;\
		CHECK_TOKEN(_type##__VA_ARGS__);\
	} while(0)

//Never use with more than 1 argument!
#define NEXT_TK_CHECK_KEYWORD(_kw, ...)\
	do {\
		GET_NEXT_TOKEN;\
		CHECK_KEYWORD(_kw##__VA_ARGS__);\
	} while(0)

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define RULE_GOOD(_rule) (_rule(pd) == SUCCESS)

#define CHECK_RULE(_rule)\
	if ((RES = _rule(pd))) { return RES; } else {}

#define NEXT_TK_CHECK_RULE(_rule)\
	do {\
		GET_NEXT_TOKEN;\
		CHECK_RULE(_rule);\
	} while(0)

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


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
		_dst->global = !pd->in_local_scope;\
		if (_err) INTERNAL_ERROR_RET;\
	} while(0)
#define ADD_ID_TYPE(_dst, _id, _type) do {\
		ADD_ID(_dst, _id);\
		_dst->type = _type;\
	} while(0)
#define ADD_PARAM(_func, _type, _qmark)\
	if (!symtable_add_param(_func, _type, _qmark)) return false;
#define ADD_CURRENT_ID(_dst) ADD_ID(_dst, TK_STR(pd->token))

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define BLOCK_NEXT_TOKEN pd->block_next_token = true
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define TOKEN_IS_TYPE_KEYWORD (KEYWORD_IS(float) || KEYWORD_IS(int) || KEYWORD_IS(string))

#define TOKEN_IS_VALUE_TYPE   (TOKEN_IS(float) 	 || TOKEN_IS(integer)   || TOKEN_IS(string) || TOKEN_IS(null)  || TOKEN_IS(ID)	 	 )






#define EMIT(_text)\
    if (!str_concat(&g_Code, (_text))) return false; else {}\
	code_generator_flush(g_CodegenOut); 

#define EMIT_NL(_text)\
        EMIT(_text"\n");

#define MAX_DIGITS 64

#define EMIT_INT(_number)                \
    do {                                \
        char _str[MAX_DIGITS];           \
        sprintf(_str, "%ld", (_number));  \
        EMIT(_str);                      \
    } while (0)



#endif //  __MACROS_H__