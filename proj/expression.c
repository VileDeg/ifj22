#include <stdio.h>
#include <stdlib.h>

#include "errors.h"
#include "codegen.h"
#include "parser.h"
#include "expression.h"
#include "macros.h"

typedef struct {
    Oper_type oper;
    DataType  type;
} Symbol;

GENERATE_VECTOR_DECLARATION(Symbol, sym);
GENERATE_VECTOR_DEFINITION (Symbol, sym);

#define TABLE_SIZE 8
// Precedence table.
int64_t precedence_table[TABLE_SIZE][TABLE_SIZE] = {
    {REDUCE, SHIFT , REDUCE, REDUCE, SHIFT, REDUCE, SHIFT, REDUCE},
    {REDUCE, REDUCE, REDUCE, REDUCE, SHIFT, REDUCE, SHIFT, REDUCE},
    {SHIFT , SHIFT , REDUCE, SHIFT , SHIFT, REDUCE, SHIFT, REDUCE},
    {SHIFT , SHIFT , REDUCE, SHIFT , SHIFT, REDUCE, SHIFT, REDUCE},
    {SHIFT , SHIFT , SHIFT , SHIFT , SHIFT, EQUAL , SHIFT, NONE  },
    {REDUCE, REDUCE, REDUCE, REDUCE, NONE , REDUCE, NONE , REDUCE},
    {REDUCE, REDUCE, REDUCE, REDUCE, NONE , REDUCE, NONE , REDUCE},
    {SHIFT , SHIFT , SHIFT , SHIFT , SHIFT, NONE  , SHIFT, NONE  }
};

Index_num index_info(Oper_type item) 
{
    switch(item) {
        case OPER_ADD:       // +
        case OPER_SUB:       // -
        case OPER_DOT:       // .
            return INDEX_0;
        case OPER_MUL:       // *
        case OPER_DIV:       // /
            return INDEX_1;
        case OPER_EQ:        // ===
        case OPER_NEQ:       // !==
            return INDEX_2;
        case OPER_LT:        // <
        case OPER_GT:        // >
        case OPER_LEQ:       // <=
        case OPER_GEQ:       // >=
            return INDEX_3;
        case OPER_LBR:       // (
            return INDEX_4;
        case OPER_RBR:       // )
            return INDEX_5;
        case DATA_ID:        // data
        case DATA_FLOAT:
        case DATA_INT:
        case DATA_STRING:
        case DATA_NULL:
            return INDEX_6;
        default:
            return INDEX_7;
    }
}

Oper_type term_info(Token* token) 
{
    Token_type tokenType = token->type;
    switch(tokenType) {
        case token_minus:
            return OPER_SUB;
        case token_plus:
            return OPER_ADD;
        case token_dot:
            return OPER_DOT;
        case token_multiply:
            return OPER_MUL;
        case token_divide:
            return OPER_DIV;
        case token_equal:
            return OPER_EQ;
        case token_not_equal:
            return OPER_NEQ;
        case token_less:
            return OPER_LT;
        case token_less_or_equal:
            return OPER_LEQ;
        case token_greater:
            return OPER_GT;
        case token_greater_or_equal:
            return OPER_GEQ;
        case token_left_bracket:
            return OPER_LBR;
        case token_right_bracket:
            return OPER_RBR;
        case token_ID:
            return DATA_ID;
        case token_integer:
            return DATA_INT;
        case token_float:
            return DATA_FLOAT;
        case token_string:
            return DATA_STRING;
        case token_null:
            return DATA_NULL;
        default:
            return OPER_DOLLAR;
    }
}

Rule_type rule_info(Symbol* oper1, Symbol* oper2, Symbol* oper3) 
{
    if (oper1->oper == OPER_LBR && oper2->oper == OPER_E && oper3->oper == OPER_RBR) 
        return RULE_BR;

    if (oper1->oper == OPER_E && oper3->oper == OPER_E) 
    {
        switch(oper2->oper) {
            case OPER_ADD:
                return RULE_ADD;
            case OPER_SUB:
                return RULE_SUB;
            case OPER_DOT:
                return RULE_DOT;
            case OPER_MUL:
                return RULE_MUL;
            case OPER_DIV:
                return RULE_DIV;
            case OPER_EQ:
                return RULE_EQ;
            case OPER_NEQ:
                return RULE_NEQ;
            case OPER_LT:
                return RULE_LT;
            case OPER_GT:
                return RULE_GT;
            case OPER_LEQ:
                return RULE_LEQ;
            case OPER_GEQ:
                return RULE_GEQ;
            default:
                return RULE_N;
        }
    }
    else
        return RULE_N;
}

int64_t implicit_conversion(Rule_type rule, DataType* dataType, Symbol* oper1, Symbol* oper2, Symbol* oper3) 
{
    (void) oper3; // 'oper3' unused in this context.

    switch(rule) 
    {
        case RULE_ID:
            if (oper1->type == TYPE_UNDEF)
                return ERROR_SEM_UNDEF_VAR;

            *dataType = oper1->type;
            break;
        case RULE_BR:
            if (oper2->type == TYPE_UNDEF)
                return ERROR_SEM_UNDEF_VAR;

            *dataType = oper2->type;
            break;
        case RULE_ADD:     
        case RULE_SUB:
        case RULE_MUL:
            if (oper1->type == TYPE_FLOAT || oper2->type == TYPE_FLOAT)
                *dataType = TYPE_FLOAT;
            else
                *dataType = TYPE_INT;
            break;
        case RULE_DIV:
            *dataType = TYPE_FLOAT;
            break;
        case RULE_DOT:
            *dataType = TYPE_STRING;
            break;
        case RULE_EQ:
        case RULE_NEQ:
            *dataType = TYPE_BOOL;
            break;
        case RULE_LT:
        case RULE_GT:
        case RULE_LEQ:
        case RULE_GEQ:
            *dataType = TYPE_BOOL;
            break;
        default:
            break;
    }
    return SUCCESS;
}

int64_t count_to_reduce(bool* reduceFound, symvec_t* stack)
{
    symelem_t* curr = stack->front;
    uint32_t cnt = 0;
    *reduceFound = false;

    while (curr)
    {
        if (curr->data.oper == OPER_REDUCE) 
        {
            *reduceFound = true;
            break;
        }
            
        curr = curr->prev;
        ++cnt;
    }
    return cnt;
}

int64_t reduce(ParserData* pd, symvec_t* stack) 
{
    (void) pd; // 'pd' unused, passed for compatibility.

    Rule_type RuleType;
    
    DataType dataType = TYPE_UNDEF;

    bool reduceFound = false;

    int64_t symbolCnt = count_to_reduce(&reduceFound, stack);

    Symbol *oper1 = NULL, *oper2 = NULL, *oper3 = NULL;
    if (symbolCnt == 1 && reduceFound) 
    {   
        oper1 = &stack->front->data;
        if ((oper1->oper == DATA_ID    ) || (oper1->oper == DATA_INT ) || (oper1->oper == DATA_FLOAT) || 
            (oper1->oper == DATA_STRING) || (oper1->oper == DATA_NULL))
            RuleType = RULE_ID;
        else
            RuleType = RULE_N;
    }
    else if (symbolCnt == 3 && reduceFound)
    {
        oper1 = &stack->front->prev->prev->data;
        oper2 = &stack->front->prev->data;
        oper3 = &stack->front->data;
        RuleType = rule_info(oper1, oper2, oper3);
    } 
    else 
        return ERROR_SYNTAX;

    if (RuleType == RULE_N)
        return ERROR_SYNTAX;

    int64_t res = SUCCESS;
    if ((res = implicit_conversion(RuleType, &dataType, oper1, oper2, oper3)) != SUCCESS)
        return res;

    { CODEGEN(emit_operator_call, RuleType); }
    
    ++symbolCnt;
    while(symbolCnt--) 
        symvec_pop_front(stack);

    Symbol symbol = { OPER_E, dataType };
    INTERNAL(symvec_push_front(stack, symbol));

    return SUCCESS;
}

DataType type_info(ParserData* pd, int64_t* errcode)
{
    TData* data = NULL;
    if (pd->token.type == token_ID)
    {
        if (pd->token.string.ptr[0] != '$')
        {
            *errcode = ERROR_SYNTAX;
            return TYPE_UNDEF;
        }
        data = symtable_find(
            pd->in_local_scope ? &pd->localTable : &pd->globalTable, 
            pd->token.string.ptr);
        
        if (data)
            return data->type;

        if (!data || (pd->var_not_yet_def && pd->lhs_var == data))
        {
            *errcode = ERROR_SEM_UNDEF_VAR;
            return TYPE_UNDEF;
        }
    }
    else
    {
        switch(pd->token.type)
        {
            case token_integer: return TYPE_INT   ;    
            case token_float  : return TYPE_FLOAT ;  
            case token_string : return TYPE_STRING; 
            case token_null   : return TYPE_NULL  ;   
            default:            return TYPE_UNDEF ;
        }
    }
    return TYPE_UNDEF;
}

Symbol* stack_get_top_term(symvec_t* stack) 
{
    for(symelem_t *tmp = stack->front; tmp != NULL; tmp = tmp->prev) 
    {
        if(tmp->data.oper < OPER_REDUCE)
            return &tmp->data;
    }
    return NULL;
}

bool stack_push_after_top_term(symvec_t* stack, Symbol sym)
{
    symelem_t* prev = NULL;

	for (symelem_t* curr = stack->front; curr != NULL; curr = curr->prev)
	{
		if (curr->data.oper < OPER_REDUCE)
		{
			symelem_t* new = malloc(sizeof(symelem_t));

			if (new == NULL)
				return false;

			new->data.oper = sym.oper;
			new->data.type = sym.type;

			if (prev == NULL)
			{
				new->prev = stack->front;
				stack->front = new;
			}
			else
			{
				new->prev = prev->prev;
				prev->prev = new;
			}
			return true;
		}
		prev = curr;
	}
	return false;
}

#define RES result

#define ERROR_RET(_retcode, ...) do {\
		symvec_dispose(&stack, NULL);\
        PRINT_ERROR_RET(_retcode, __VA_ARGS__);\
		return _retcode;\
	} while(0)

#define RESULT(_funccall, ...)\
    if ((RES = _funccall)) ERROR_RET(RES, __VA_ARGS__); else {} 

int64_t expression(ParserData* pd) 
{
    RULE_OPEN;
    {
        symvec_t stack;
        symvec_init(&stack);

        Symbol symbol = { OPER_DOLLAR, TYPE_UNDEF };
        INTERNAL(symvec_push_front(&stack, symbol));
        
        // [x,y] = indices for precedence table
        Oper_type currSymbol, stackSymbol;
        Symbol* stackTerm;

        bool success = false;
        int64_t type_info_errcode = SUCCESS;

        do
        {
            currSymbol = term_info(&pd->token);
            INTERNAL(stackTerm = stack_get_top_term(&stack));
            stackSymbol = stackTerm->oper;
            (void)stackSymbol;

            DataType symbolDataType = type_info(pd, &type_info_errcode);
            if (type_info_errcode != SUCCESS)
                ERROR_RET(type_info_errcode, "type_info error.");

            Index_num xIndex = index_info(stackTerm->oper);
            Index_num yIndex = index_info(currSymbol);
            Sign_type signType = precedence_table[xIndex][yIndex];
            switch(signType) 
            {
                case SHIFT:
                    symbol.oper = OPER_REDUCE;
                    symbol.type = TYPE_UNDEF;
                    INTERNAL(stack_push_after_top_term(&stack, symbol));

                    symbol.oper = currSymbol;
                    symbol.type = symbolDataType;
                    INTERNAL(symvec_push_front(&stack, symbol));

                    if (currSymbol == DATA_ID     || currSymbol == DATA_INT  || currSymbol == DATA_FLOAT || 
                        currSymbol == DATA_STRING || currSymbol == DATA_NULL)
                    {
                        if (currSymbol == DATA_ID)
                            { CODEGEN(emit_check_var_defined, pd->token.string.ptr, pd->in_local_scope); }
                        CODEGEN(emit_push_token, pd->token, pd->in_local_scope);
                    }
                    GET_NEXT_TOKEN;
                    break;
                case EQUAL:
                    symbol.oper = currSymbol;
                    symbol.type = symbolDataType;
                    INTERNAL(symvec_push_front(&stack, symbol));
                    GET_NEXT_TOKEN;
                    break;
                case REDUCE:
                    RESULT(reduce(pd, &stack));
                    break;
                case NONE:
                    if (currSymbol == stackTerm->oper && currSymbol == OPER_DOLLAR) 
                        success = true;
                    else 
                        ERROR_RET(ERROR_SYNTAX, "expression error.");
                    break;
            }
        }
        while (!success);

        Symbol* lastNonterm = &stack.front->data;

        VILE_ASSERT(lastNonterm, "Unexpected.");
        if (lastNonterm->oper != OPER_E)
            ERROR_RET(ERROR_SYNTAX, "last nonterm is not E.");

        if (pd->lhs_var)
        {
            pd->lhs_var->type = lastNonterm->type;
            { CODEGEN(emit_pop_expr_result, pd->lhs_var->id, pd->lhs_var->global ? "GF" : "LF"); }
        }

        symvec_dispose(&stack, NULL);

        BLOCK_NEXT_TOKEN; //We use this to avoid reading next token.
    }
    RULE_CLOSE;
}