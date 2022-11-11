#include <stdio.h>
#include <stdlib.h>

#include "precedence_t.h"
#include "errors.h"
#include "code_generator.h"

// Precedence table.
int64_t precedent_table[TAB_SIZE][TAB_SIZE] = 
{
    {SIGN_R, SIGN_L, SIGN_R, SIGN_R, SIGN_L, SIGN_R, SIGN_L, SIGN_L},
    {SIGN_R, SIGN_R, SIGN_R, SIGN_R, SIGN_L, SIGN_R, SIGN_L, SIGN_R},
    {SIGN_L, SIGN_L, SIGN_R, SIGN_L, SIGN_L, SIGN_R, SIGN_L, SIGN_R},
    {SIGN_L, SIGN_L, SIGN_R, SIGN_L, SIGN_L, SIGN_R, SIGN_L, SIGN_R},
    {SIGN_L, SIGN_L, SIGN_L, SIGN_L, SIGN_L, SIGN_E, SIGN_L, SIGN_N},
    {SIGN_R, SIGN_R, SIGN_R, SIGN_R, SIGN_N, SIGN_R, SIGN_N, SIGN_R},
    {SIGN_R, SIGN_R, SIGN_R, SIGN_R, SIGN_N, SIGN_R, SIGN_N, SIGN_R},
    {SIGN_L, SIGN_L, SIGN_L, SIGN_L, SIGN_L, SIGN_N, SIGN_L, SIGN_N}
};

stackSymbol stack;                                                              // FIXME Global :(((
bool flag_stop;

void stack_init(stackSymbol* stack) 
{
    stack->top = NULL;
}

bool stack_push(stackSymbol* stack, Oper_type item, Data_type elementType) 
{
    elementSymbol *new = malloc(sizeof(elementSymbol));                             // TODO kontrola na NULL
    if (new == NULL)
        return false;
    new->item = item;
    new->next = NULL;
    new->elementType = elementType;
    if(stack->top != NULL) {
        new->next = stack->top;
        stack->top = new;

    } else {
        stack->top = new;
    }
    return true;
    
    // elementSymbol *new = malloc(sizeof(elementSymbol));
    // if(new == NULL) {
    //     return false;
    // }
    // new->item = item;
    // new->elementType = elementType;
    // new->next = stack->top;
    // return true;
    
    // new->next = NULL;
    // if(stack->top != NULL) {
    //     new->next = stack->top;
    //     stack->top = new;
    // } else {
    //     stack->top = new;
    // }
}

void stack_pop(stackSymbol* stack) 
{
    if(stack->top->next != NULL) {
        elementSymbol *tmp = stack->top;
        stack->top = tmp->next;
        free(tmp);
    } else {
        free(stack->top);
        stack->top = NULL;
    }
}

void stack_clear(stackSymbol* stack)
{
    while(stack->top != NULL) {
        stack_pop(stack);
    }
}

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
            return INDEX_6;
        // case OPER_DOLLAR: // $
        //     return INDEX_7;
        default:
            return INDEX_7;
    }
}

Oper_type term_info(Token* token) {
    Token_types tokenType = token->type;
    switch(tokenType) {
    // switch(token->type) {
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
        // case token_dollar:
        //     return OPER_DOLLAR;
        default:
            return OPER_DOLLAR;
    }
}
elementSymbol* term_top(stackSymbol* stack) {
    elementSymbol *tmp = stack->top;
    // while(tmp != NULL) {
    //     if(tmp->item < OPER_RED)
    //         return tmp;
    // }
    // return NULL;
    for(elementSymbol *tmp = stack->top; tmp != NULL; tmp = tmp->next) {
        if(tmp->item < OPER_RED)
            return tmp;
    }
    return NULL;
}

elementSymbol* nonterm_top(stackSymbol* stack) {
    return stack->top;
}

Data_type type_info(Token* token, ParserData* pd) {
    TData *item;
    Token_types tokenType = token->type;
    switch(tokenType) {
        case token_ID:
            item = symtable_find(&pd->localTable, pd->token.value.String->ptr);
            if(item != NULL) {
                return item->type;
            } else {
                // fprintf(stderr, "ERROR: Variable %s is not defined.\n", token->data);
                return TYPE_NONE;
            }
        case token_integer:
            return TYPE_INT;
        case token_float:
            return TYPE_FLOAT;
        case token_string:
            return TYPE_STRING;
        default:
            return TYPE_NONE;
    }
}


Rule_type rule_info(elementSymbol* oper_third, elementSymbol* oper_second, elementSymbol* oper_first, int64_t cnt) {
    if(oper_third->item == OPER_E && oper_first->item == OPER_E) {
        switch(oper_second->item) {
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
    } else {
        return RULE_N;
    }
    if(oper_first->item == OPER_RBR && oper_second->item == OPER_E && oper_first->item == OPER_LBR) {
        return RULE_BR;
    } else {
        return RULE_N;
    }
}

uint64_t reduce(ParserData* pd) {
    int64_t cnt;
    Rule_type rule;
    elementSymbol* topTerm = nonterm_top(&stack);
    Data_type type;

    for(cnt = 0; topTerm != NULL; cnt++) {
        if(topTerm->item == OPER_RED) {
            flag_stop = true;
            break;
        } else {
            flag_stop = false;
        }
        topTerm = topTerm->next;
    }

    if(flag_stop) {
        elementSymbol* oper_first = stack.top;
        if(cnt == 1) {   
            if((oper_first->item == DATA_ID) || (oper_first->item == DATA_INT) || (oper_first->item == DATA_FLOAT) || (oper_first->item == DATA_STRING)) {
                rule = RULE_ID;
            } else {
                rule = RULE_N;
            }
        } else if(cnt == 3) {
            elementSymbol* oper_second = stack.top->next;
            elementSymbol* oper_third = stack.top->next->next;
            rule = rule_info(oper_third, oper_second, oper_first, cnt);
        } else {
            return ERROR_SYNTAX;
        }
        
        // if(rule == RULE_N) {
        //     return ERROR_SYNTAX;
        // } else {

        // }

        //if(rule_info == RULE_ADD && data_type == STRING) => GEN_CODE(generate_stack_operation, RULE_DOT)      // GENERATE CODE
        if(cnt != 0) {
            while(cnt != 0) {
            stack_pop(&stack);
            cnt--;
            }
        } else {
            stack_pop(&stack);
        }
        stack_push(&stack, OPER_E, type);
    }
    return REDUCE_OK;
}

int64_t expression_parsing(ParserData* pd) {
    bool flag_final = false;

    // [x,y] = index for precedence table
    elementSymbol* ySym;
    Oper_type xSym;
   
    stack_init(&stack);
    if((stack_push(&stack, OPER_DOLLAR, TYPE_NONE)) == false) {
        stack_clear(&stack);
    return ERROR_INTERNAL;
    }
    //stack_push(&stack, OPER_DOLLAR, TYPE_NONE);

    while(!flag_final) {

        // get index, get symbol from terminal
        xSym = term_info(&pd->token);

        // get top terminal
        ySym = term_top(&stack);

        Data_type type = type_info(&pd->token, pd);

        switch(precedent_table[index_info(ySym->item)][index_info(xSym)]) {
            case SIGN_R:
                reduce(pd);
                break;
            case SIGN_E:
                stack_push(&stack, xSym, type);
                if(scanner_get_next_token(&pd->token)) {
                    stack_clear(&stack);
                    return ERROR_INTERNAL;
                }
                break;
            case SIGN_L: ;
                elementSymbol* topBefore = NULL;
                elementSymbol* topAfter = (&stack)->top;
                while(topAfter != NULL) {
                    if(topAfter->item < OPER_RED) {
                        elementSymbol* new = (elementSymbol*)malloc(sizeof(elementSymbol));
                        if(new == NULL) {
                            stack_clear(&stack);
                            return ERROR_INTERNAL;
                        }
                        new->item = OPER_RED;
                        new->elementType = TYPE_NONE;
                        if(topBefore != NULL) {
                            new->next = topBefore->next;
                            topBefore->next = new;
                        } else {
                            new->next = (&stack)->top;
                            (&stack)->top = new;
                        }
                    }
                    
                    topBefore = topAfter;
                    topAfter = topAfter->next;
                }

                stack_push(&stack, xSym, type);

                if (xSym == DATA_ID || xSym == DATA_INT || xSym == DATA_FLOAT || xSym == DATA_STRING) {
                    GEN_CODE(emit_push, pd->token);
                }
                if(scanner_get_next_token(&pd->token)) {
                    stack_clear(&stack);
                    return ERROR_INTERNAL;
                }
                break;
            case SIGN_N:
                if(xSym == ySym->item && xSym == OPER_DOLLAR) {
                    flag_final = true;
                } else {
                    stack_clear(&stack);
                    return ERROR_SYNTAX;
                }
                break;
        }
        
    }

    elementSymbol* E = nonterm_top(&stack);

    char *frame = "LF";
    if(!pd->in_local_scope) frame = "GF";

    if(pd->lhs_var != NULL) {
        switch (pd->lhs_var->type) {
        case TYPE_FLOAT:
            GEN_CODE(emit_stack_pop_res, pd->lhs_var->id, E->elementType, TYPE_FLOAT, frame);
            break;
        case TYPE_INT:
            GEN_CODE(emit_stack_pop_res, pd->lhs_var->id, E->elementType, TYPE_INT, frame);
            break;
        case TYPE_STRING:
            GEN_CODE(emit_stack_pop_res, pd->lhs_var->id, TYPE_STRING, TYPE_STRING, frame);
            break;
        case TYPE_BOOL:
            GEN_CODE(emit_stack_pop_res, pd->lhs_var->id, E->elementType, TYPE_BOOL, frame);
            break;
        case TYPE_NIL:
            GEN_CODE(emit_stack_pop_res, pd->lhs_var->id, E->elementType, TYPE_NIL, frame);
            break;
        default:
            break;
        }
    }

    stack_clear(&stack);
    return EXPRESSION_OK;

}