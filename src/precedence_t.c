#include <stdio.h>
#include <stdlib.h>

#include "precedence_t.h"
#include "scanner.h"
#include "symtable.h"
#include "string_t.h"
#include "parser.h"

#define FLAG_FINAL 0;

// Global :(((                                                                  // FIXME
int64_t index = 42; 

void stack_init(symStack* stack) 
{
    stack->top = NULL;
}

void stack_push(symStack* stack, Oper_type item) 
{
    symbolPtr new = malloc(sizeof(Oper_type));                                  // TODO kontrola na NULL
    new->item = item;
    new->next = NULL;
    if(stack->top != NULL) {
        new->next = stack->top;
        stack->top = new;
    } else {
        stack->top = new;
    }
}

void stack_pop(symStack* stack) 
{
    if(stack->top->next != NULL) {
        symbolPtr tmp = stack->top;
        stack->top = tmp->next;
        free(tmp);
    } else {
        free(stack->top);
        stack->top = NULL;
    }
}

void stack_clear(symStack* stack)
{
    while(stack->top != NULL) {
        stack_pop(stack);
    }
}
// split? index switch, type switch? 
Oper_type token_info(Token* token) {
    Token_types tokenType = token.type;
    switch(tokenType) {
        case token_minus:
            return OPER_SUB;
        case token_plus:
            return OPER_ADD;
        case token_dot:
            return OPER_DOT;
            index = 0;
            break;
        case token_multiply:
            return OPER_MUL;
        case token_divide:
            return OPER_DIV;
            index = 1;
            break;
        case token_equal:
            return OPER_EQ;
        case token_not_equal:
            return OPER_NEQ;
            index = 2;
            break;
        case token_less:
            return OPER_LT;
        case token_less_or_equal:
            return OPER_LEQ;
        case token_greater:
            return OPER_GT;
        case token_greater_or_equal:
            return OPER_GEQ;
            index = 3;
            break;
        case token_left_bracket:
            return OPER_LBR;
        case token_right_bracket:
            return OPER_RBR;
            index = 4;
            break;
        case token_ID:
        case token_integer:
        case token_float:
        case token_exponent:
        case token_string:
            return OPER_ID;
            index = 5;
            break;
        case token_dollar:
            return OPER_DOLLAR;
            index = 6;
            break;
        default:
            return OPER_DOLLAR;
            // error or smth?
            break;
    }
    return index;
}
symbolPtr token_top(symStack* stack) {
    symbolPtr tmp = stack->top;
    while(tmp != NULL) {
        if(tmp->item < OPER_RED)
            return tmp;
    }
    return NULL;
}

void reduce(symbolPtr* ySym, symStack stack, Oper_type xSym) {
    switch(* ySym) {
        // E --> id
        case OPER_ID:
            stack_pop(stack);
            * ySym = stack->top->item;
            stack_push(stack, OPER_E);
            break;
        case OPER_RBR:
            if(stack_pop(stack) == OPER_RBR) {
                if(stack_pop(stack) == OPER_E) {
                    if(stack_pop(stack) == OPER_LBR) {
                        * ySym = stack->top->item;
                        stack_push(stack, OPER_E);
                    }
                }
            }
        case OPER_ADD:
        case OPER_SUB:
        case OPER_DOT:
        case OPER_MUL:
        case OPER_DIV:
        case OPER_EQ:
        case OPER_NEQ:
        case OPER_LT:
        case OPER_GT:
        case OPER_LEQ:
        case OPER_GEQ:
            reduce_binary(ySym, stack); // +RULE?
    }
}

void expression_parsing(ParserData* pd) {
    symStack stack;
    // [x,y] = index for precedence table
    Oper_type xSym;
    symbolPtr ySym;
    stack_init(&stack);
    stack_push(&stack, OPER_DOLLAR);
    while(!FLAG_FINAL) {
        // index, symbol from token
        xSym = token_info(&pd->token);
        // get top token
        ySym = token_top(&stack);
        switch(precedent_table[token_info(ySym->item)[token_info(xSym)]]) {
            case SIGN_R:
                reduce(&ySym, &stack, &xSym);
            case SIGN_E:
                stack_push(&stack, xSym);
                break;
            case SIGN_L:
            // SHIFT 
            case SIGN_N:
                if(xSym == ySym && xSym == DOLLAR)
                    FLAG_FINAL = 1;
                break;
        }
    }
}