#include <stdio.h>
#include <stdlib.h>

#include "symbol_stack.h"
#include "errors.h"
#include "code_generator.h"
#include "parser.h"

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
    if(oper1->operType == OPER_LBR && oper2->operType == OPER_E && oper3->operType == OPER_RBR) 
        return RULE_BR;

    if(oper1->operType == OPER_E && oper3->operType == OPER_E) 
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
uint32_t implicit_conversion(Rule_type rule, DataType* dataType, bool* type_equal, Symbol* oper1, Symbol* oper2, Symbol* oper3) 
{
    switch(rule) 
    {
        case RULE_ID:
            if(oper1->dataType == TYPE_UNDEF)
                return ERROR_SEM_UNDEF_VAR;

            *dataType = oper1->dataType;
            break;
        case RULE_BR:
            if(oper2->dataType == TYPE_UNDEF)
                return ERROR_SEM_UNDEF_VAR;

            *dataType = oper2->dataType;
            break;
        case RULE_ADD:                                                          
        case RULE_SUB:
        case RULE_MUL:
            if(oper1->dataType == TYPE_INT && oper3->dataType == TYPE_INT)
                *dataType = TYPE_INT;
            else if(oper1->dataType == TYPE_FLOAT && oper3->dataType == TYPE_FLOAT)
                *dataType = TYPE_FLOAT;
            else if(oper1->dataType == TYPE_INT && oper3->dataType == TYPE_FLOAT)
            {
                *dataType = TYPE_FLOAT;
                CODEGEN(emit_stack_top_int2float);
            }
            else if(oper1->dataType == TYPE_FLOAT && oper3->dataType == TYPE_INT)
            {
                *dataType = TYPE_FLOAT;
                CODEGEN(emit_stack_sec_int2float);
            }
            else
                return ERROR_SEM_EXPRESSION;
            break;
        case RULE_DIV:
            if(oper1->dataType == TYPE_INT && oper3->dataType == TYPE_FLOAT) 
                CODEGEN(emit_stack_top_int2float);
            else if(oper1->dataType == TYPE_FLOAT && oper3->dataType == TYPE_INT) 
                CODEGEN(emit_stack_sec_int2float);
            else 
                return ERROR_SEM_EXPRESSION;
            *dataType = TYPE_FLOAT;
            break;
        case RULE_DOT:
            if(oper1->dataType != TYPE_STRING || oper3->dataType != TYPE_STRING)
                return ERROR_SEM_EXPRESSION;
            *dataType = TYPE_STRING;
            break;
        case RULE_EQ:
        case RULE_NEQ:
            *type_equal = (oper1->dataType == oper3->dataType);
            *dataType = TYPE_BOOL;
            break;
        case RULE_LT:
        case RULE_GT:
        case RULE_LEQ:
        case RULE_GEQ:
            *dataType = TYPE_BOOL;
            // if((oper1->dataType != TYPE_BOOL || oper3->dataType != TYPE_BOOL) || 
            // oper3->dataType != TYPE_STRING && oper1->dataType != TYPE_STRING) {
            //     return ERROR_SEM_EXPRESSION;
            //}
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
        if(curr->operType == OPER_REDUCE) 
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
    
    DataType dataType;

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

    bool type_equal = false; // <-- for '===' and '!==' operators
    if (implicit_conversion(RuleType, &dataType, &type_equal, oper1, oper2, oper3) != SUCCESS)
        return ERROR_SYNTAX;

    if (RuleType == RULE_EQ || RuleType == RULE_NEQ)
    {
        if (RuleType == RULE_EQ && type_equal || RuleType == RULE_NEQ && !type_equal)
            { CODEGEN(emit_stack_operation, RuleType); }
        else
        {
            { CODEGEN(emit_clear_stack); }
            { CODEGEN(emit_push_bool_literal, type_equal); }
        }
    }
    else
        { CODEGEN(emit_stack_operation, RuleType); }
    
    stack_pop_count(stack, SymbolCnt + 1);

    stack_push(stack, OPER_E, dataType);

    return SUCCESS;
}

#define RES result

#define ERROR_RET(_retcode) \
    do { \
		stack_clear(&stack); \
		return _retcode; \
	} while(0)

#define RESULT(_funccall) \
    if ((RES = _funccall)) \
        ERROR_RET(RES); \
    else {} 

#define INTERNAL(_expr) \
    if (!(_expr)) \
        ERROR_RET(ERROR_INTERNAL);

#define TYPE_INFO(_ret)\
    TData* _data = NULL;\
    if (pd->token.type == token_ID)\
    {\
        if (pd->token.string->ptr[0] != '$')\
            ERROR_RET(ERROR_SYNTAX);\
        _data = symtable_find(pd->in_local_scope ? &pd->localTable : &pd->globalTable, pd->token.string->ptr);\
        if(!_data || pd->var_not_yet_def)\
            ERROR_RET(ERROR_SEM_UNDEF_VAR);\
        _ret = _data->type;\
    }\
    else\
    {\
        switch(pd->token.type)\
        {\
            case token_integer: _ret = TYPE_INT;    break;\
            case token_float:   _ret = TYPE_FLOAT;  break;\
            case token_string:  _ret = TYPE_STRING; break;\
            case token_null:    _ret = TYPE_NULL;   break;\
            default:            _ret = TYPE_UNDEF;\
        }\
    }

DataType type_info(ParserData* pd, int* errcode)
{
    TData* data = NULL;
    if (pd->token.type == token_ID)
    {
        if (pd->token.string->ptr[0] != '$')
        {
            *errcode = ERROR_SYNTAX;
            return TYPE_UNDEF;
        }
        data = symtable_find(pd->in_local_scope ? &pd->localTable : &pd->globalTable, pd->token.string->ptr);
        if(!data || pd->var_not_yet_def)
        {
            *errcode = ERROR_SEM_UNDEF_VAR;
            return TYPE_UNDEF;
        }
        return data->type;
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
    int64_t RES;

    SymbolStack stack;
    stack_init(&stack);

    INTERNAL(stack_push(&stack, OPER_DOLLAR, TYPE_UNDEF));
    
    // [x,y] = indices for precedence table
    Oper_type currSymbol;
    Oper_type stackSymbol;
    Symbol* stackTerm;

    bool success = false;

    do
    {
        currSymbol = term_info(&pd->token);
        INTERNAL(stackTerm = stack_get_top_term(&stack));
        stackSymbol = stackTerm->operType;

        int errcode = SUCCESS;
        DataType symbolDataType = type_info(pd, &errcode);
        if (errcode != SUCCESS)
            ERROR_RET(errcode);

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
                    CODEGEN(emit_push, pd->token, pd->in_local_scope);

                RESULT(scanner_get_next_token(&pd->token));
                break;
            case EQUAL:
                INTERNAL(stack_push(&stack, currSymbol, symbolDataType));
                RESULT(scanner_get_next_token(&pd->token))
                break;
            case REDUCE:
                RESULT(reduce(pd, &stack));
                break;
            case NONE:
                if(currSymbol == stackTerm->operType && currSymbol == OPER_DOLLAR) 
                    success = true;
                else 
                    ERROR_RET(ERROR_SYNTAX);
                break;
        }
    }
    while (!success);

    Symbol* lastNonterm = stack.top;

    INTERNAL(lastNonterm);
    if (lastNonterm->operType != OPER_E)
        ERROR_RET(ERROR_SYNTAX);

    if(pd->lhs_var)
    {
        pd->lhs_var->type = lastNonterm->dataType;
        { CODEGEN(emit_stack_pop_res, pd->lhs_var->id, pd->in_local_scope ? "LF" : "GF"); }
    }

    stack_clear(&stack);

    pd->block_next_token = true; // <-- we use this to avoid reading next token
    return SUCCESS;
}