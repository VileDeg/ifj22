#include <stdio.h>
#include <stdlib.h>

#include "symbol_stack.h"
#include "errors.h"
#include "code_generator.h"
#include "parser.h"

#include "macros.h"

// Precedence table.
int64_t precedence_table[TAB_SIZE][TAB_SIZE] = {
    {REDUCE, SHIFT , REDUCE, REDUCE, SHIFT, REDUCE, SHIFT, REDUCE},
    {REDUCE, REDUCE, REDUCE, REDUCE, SHIFT, REDUCE, SHIFT, REDUCE},
    {SHIFT , SHIFT , REDUCE, SHIFT , SHIFT, REDUCE, SHIFT, REDUCE},
    {SHIFT , SHIFT , REDUCE, SHIFT , SHIFT, REDUCE, SHIFT, REDUCE},
    {SHIFT , SHIFT , SHIFT , SHIFT , SHIFT, EQUAL , SHIFT, NONE  },
    {REDUCE, REDUCE, REDUCE, REDUCE, NONE , REDUCE, NONE , REDUCE},
    {REDUCE, REDUCE, REDUCE, REDUCE, NONE , REDUCE, NONE , REDUCE},
    {SHIFT , SHIFT , SHIFT , SHIFT , SHIFT, NONE  , SHIFT, NONE  }
};

Index_num index_info(Oper_type item) {
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

Oper_type term_info(Token* token) {
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
    if (oper1->operType == OPER_LBR && oper2->operType == OPER_E && oper3->operType == OPER_RBR) 
        return RULE_BR;

    if (oper1->operType == OPER_E && oper3->operType == OPER_E) 
    {
        switch(oper2->operType) {
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

uint32_t implicit_conversion(Rule_type rule, DataType* dataType, Symbol* oper1, Symbol* oper2, Symbol* oper3) 
{
    switch(rule) 
    {
        case RULE_ID:
            if (oper1->dataType == TYPE_UNDEF)
                return ERROR_SEM_UNDEF_VAR;

            *dataType = oper1->dataType;
            break;
        case RULE_BR:
            if (oper2->dataType == TYPE_UNDEF)
                return ERROR_SEM_UNDEF_VAR;

            *dataType = oper2->dataType;
            break;
        case RULE_ADD:     
        case RULE_SUB:
        case RULE_MUL:
            if (oper1->dataType == TYPE_FLOAT || oper2->dataType == TYPE_FLOAT)
                *dataType = TYPE_FLOAT;
            else
                *dataType = TYPE_INT;
            break;
        case RULE_DIV:
            *dataType = TYPE_FLOAT;
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

uint32_t num_of_symbols_to_reduce(bool* reduceFound, SymbolStack* stack)
{
    Symbol* curr = stack->top;
    uint32_t cnt = 0;
    *reduceFound = false;
    for(; curr != NULL; cnt++) 
    {
        if (curr->operType == OPER_REDUCE) 
        {
            *reduceFound = true;
            break;
        }
            
        curr = curr->next;
    }
    return cnt;
}

uint64_t reduce(ParserData* pd, SymbolStack* stack) 
{
    Rule_type RuleType;
    
    DataType dataType = TYPE_UNDEF;

    bool reduceFound = false;

    int64_t SymbolCnt = num_of_symbols_to_reduce(&reduceFound, stack);

    Symbol* oper1 = NULL;
    Symbol* oper2 = NULL;
    Symbol* oper3 = NULL;
    if (SymbolCnt == 1 && reduceFound) 
    {   
        oper1 = stack->top;
        if ((oper1->operType == DATA_ID)     || (oper1->operType == DATA_INT) || (oper1->operType == DATA_FLOAT) || 
            (oper1->operType == DATA_STRING) || (oper1->operType == DATA_NULL))
            RuleType = RULE_ID;
        else
            RuleType = RULE_N;
    }
    else if (SymbolCnt == 3 && reduceFound)
    {
        oper1 = stack->top->next->next;
        oper2 = stack->top->next;
        oper3 = stack->top;
        RuleType = rule_info(oper1, oper2, oper3);
    } 
    else 
        return ERROR_SYNTAX;

    if (RuleType == RULE_N)
        return ERROR_SYNTAX;

    int res = SUCCESS;
    if ((res = implicit_conversion(RuleType, &dataType, oper1, oper2, oper3)) != SUCCESS)
        return res;

    { CODEGEN(emit_stack_operation, RuleType); }
    
    stack_pop_count(stack, SymbolCnt + 1);

    stack_push(stack, OPER_E, dataType);

    return SUCCESS;
}

#if 1
#define RES result

#define ERROR_RET(_retcode, ...) \
    do { \
		stack_clear(&stack); \
        PRINT_ERROR_RET(_retcode, __VA_ARGS__);\
		return _retcode; \
	} while(0)

#define RESULT(_funccall, ...) \
    if ((RES = _funccall)) \
        ERROR_RET(RES, __VA_ARGS__); \
    else {} 
#endif

DataType type_info(ParserData* pd, int* errcode)
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
            VILE_ASSERT(false, "");
            *errcode = ERROR_SEM_UNDEF_VAR;
            return TYPE_UNDEF;
        }
    }
    else
    {
        switch(pd->token.type)
        {
            case token_integer: return TYPE_INT;    
            case token_float:   return TYPE_FLOAT;  
            case token_string:  return TYPE_STRING; 
            case token_null:    return TYPE_NULL;   
            default:            return TYPE_UNDEF;
        }
    }
}

int64_t expression_parsing(ParserData* pd) 
{
    RULE_OPEN;
    {
        SymbolStack stack;
        stack_init(&stack);

        INTERNAL(stack_push(&stack, OPER_DOLLAR, TYPE_UNDEF));
        
        // [x,y] = indices for precedence table
        Oper_type currSymbol;
        Oper_type stackSymbol;
        Symbol* stackTerm;

        bool success = false;
        int type_info_errcode = SUCCESS;

        do
        {
            currSymbol = term_info(&pd->token);
            INTERNAL(stackTerm = stack_get_top_term(&stack));
            stackSymbol = stackTerm->operType;

            DataType symbolDataType = type_info(pd, &type_info_errcode);
            if (type_info_errcode != SUCCESS)
                ERROR_RET(type_info_errcode, "type_info error.");

            Index_num xIndex = index_info(stackTerm->operType);
            Index_num yIndex = index_info(currSymbol);
            Sign_type signType = precedence_table[xIndex][yIndex];
            switch(signType) 
            {
                case SHIFT:
                    INTERNAL(stack_push_after_top_term(&stack, OPER_REDUCE, TYPE_UNDEF));

                    INTERNAL(stack_push(&stack, currSymbol, symbolDataType));

                    if (currSymbol == DATA_ID || currSymbol == DATA_INT || currSymbol == DATA_FLOAT || 
                        currSymbol == DATA_STRING || currSymbol == DATA_NULL)
                    {
                        if (currSymbol == DATA_ID)
                            { CODEGEN(emit_check_var_defined, pd->token.string.ptr, pd->in_local_scope); }
                        CODEGEN(emit_push, pd->token, pd->in_local_scope);
                    }
                    GET_NEXT_TOKEN;
                    break;
                case EQUAL:
                    INTERNAL(stack_push(&stack, currSymbol, symbolDataType));
                    GET_NEXT_TOKEN;
                    break;
                case REDUCE:
                    RESULT(reduce(pd, &stack));
                    break;
                case NONE:
                    if (currSymbol == stackTerm->operType && currSymbol == OPER_DOLLAR) 
                        success = true;
                    else 
                        ERROR_RET(ERROR_SYNTAX, "expression_parsing error.");
                    break;
            }
        }
        while (!success);

        Symbol* lastNonterm = stack.top;

        INTERNAL(lastNonterm);
        if (lastNonterm->operType != OPER_E)
            ERROR_RET(ERROR_SYNTAX, "last nonterm is not E.");

        if (pd->lhs_var)
        {
            DataType type = lastNonterm->dataType;

            pd->lhs_var->type = type;

            { CODEGEN(emit_stack_pop_res, pd->lhs_var->id, pd->lhs_var->global ? "GF" : "LF"); }

            if (pd->in_if_while)
            {
                { CODEGEN(emit_expression_bool_convert); }
            }
        }

        stack_clear(&stack);

        pd->block_next_token = true; // <-- we use this to avoid reading next token
    }
    RULE_CLOSE;
}