#include <stdio.h>
#include <stdlib.h>

#include "precedence_t.h"
#include "errors.h"

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

void stack_push(stackSymbol* stack, Oper_type item) 
{
    elementSymbol *new = malloc(sizeof(elementSymbol));                             // TODO kontrola na NULL
    new->item = item;
    new->next = NULL;
    if(stack->top != NULL) {
        new->next = stack->top;
        stack->top = new;
    } else {
        stack->top = new;
    }
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
        case OPER_ID:        // id
            return INDEX_6;
        case OPER_DOLLAR:    // $
            return INDEX_7;
        // case default some error check?
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
        case token_integer:
        case token_float:
        //case token_exponent:
        case token_string:
            return OPER_ID;
        case token_dollar:
            return OPER_DOLLAR;
        default:
            return OPER_DOLLAR;
            // case default some error check?
    }
}
elementSymbol* term_top(stackSymbol* stack) {
    elementSymbol *tmp = stack->top;
    while(tmp != NULL) {
        if(tmp->item < OPER_RED)
            return tmp;
    }
    return NULL;
}

elementSymbol* nonterm_top(stackSymbol* stack) {
    return stack->top;
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
    } else if(oper_first->item == OPER_RBR && oper_second->item == OPER_E && oper_first->item == OPER_LBR) {
        return RULE_BR;
    } else {
        return RULE_N;
    }
}

bool reduce(ParserData* pd) {
    int64_t cnt;
    Rule_type rule;
    elementSymbol* topTerm = term_top(&stack);
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
        if(cnt == 1) {                                                          // SYMTABLE/GENERATE CODE ?
            rule = RULE_ID;                                                     // FIX ME check if (item = variable/constant)
        } else if(cnt == 3) {
            elementSymbol* oper_second = stack.top->next;
            elementSymbol* oper_third = stack.top->next->next;
            rule = rule_info(oper_third, oper_second, oper_first, cnt);
        } else {
            rule = RULE_N;
        }                                                                       // TODO check semantics

        //if(rule_info == RULE_ADD && data_type == STRING) => GEN_CODE(generate_stack_operation, RULE_DOT)      // GENERATE CODE
        if(cnt != 0) {
            while(cnt != 0) {
            stack_pop(&stack);
            cnt--;
            }
        } else {
            stack_pop(&stack);
        }
        stack_push(&stack, OPER_E);
    }
}

int64_t expression_parsing(ParserData* pd) {
    bool flag_final = false;

    // [x,y] = index for precedence table
    elementSymbol* ySym;
    Oper_type xSym;

    stack_init(&stack);
    stack_push(&stack, OPER_DOLLAR);

    while(!flag_final) {

        // get index, get symbol from terminal
        xSym = term_info(&pd->token);

        // get top terminal
        ySym = term_top(&stack);

        switch(precedent_table[index_info(ySym->item)][index_info(xSym)]) {
            case SIGN_R:
                reduce(pd);
                break;
            case SIGN_E:
                stack_push(&stack, xSym);
                break;
            case SIGN_L:
                // check if (xSym = variable/constant) => GEN_CODE(generate_push, pd->token);              // GENERATE CODE
                break;
            case SIGN_N:
                if(xSym == ySym->item && xSym == OPER_DOLLAR)
                    flag_final = true;
                break;
        }
        
    }

    elementSymbol* E = nonterm_top(&stack);

    char *frame = "LF";
    //if(pd->lhs_var->global) frame = "GF";                                                                 // don't know how to get this info (global/local)

    // if(pd->lhs_var != NULL) {
    //     switch (pd->lhs_var->type) {
    //     case TYPE_FLOAT:
    //         GEN_CODE(generate_stack_pop_res, pd->lhs_var->id, E->elementType, TYPE_FLOAT, frame);           // FIX ME !!! generate_stack_pop_res is not defined
    //     case TYPE_INT:
    //         GEN_CODE(generate_stack_pop_res, pd->lhs_var->id, E->elementType, TYPE_INT, frame);
    //     case TYPE_STRING:
    //         GEN_CODE(generate_stack_pop_res, pd->lhs_var->id, TYPE_STRING, TYPE_STRING, frame);
    //     case TYPE_BOOL:
    //         GEN_CODE(generate_stack_pop_res, pd->lhs_var->id, E->elementType, TYPE_BOOL, frame);
    //     case TYPE_NIL:
    //         GEN_CODE(generate_stack_pop_res, pd->lhs_var->id, E->elementType, TYPE_NIL, frame);
    //     default:
    //         break;
    //     }
    // }

}