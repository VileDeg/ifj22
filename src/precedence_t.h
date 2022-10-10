#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define TAB_SIZE 9

/**
 * @enum Operators of the precedence table.
 */
typedef enum {
    OPER_ADD,       // +
    OPER_SUB,       // - 
    OPER_DOT,       // .
    OPER_MUL,       // *
    OPER_DIV,       // /
    OPER_CAR,       // ^
    OPER_EQ,        // ===
    OPER_NEQ,       // !==
    OPER_LT,        // <
    OPER_GT,        // >
    OPER_LEQ,       // <=
    OPER_GEQ,       // >=
    OPER_LBR,       // (
    OPER_RBR,       // )
    OPER_ID,        // id
    OPER_DOLLAR,    // $
    OPER_RED        // Reduce
} Oper_type;

/**
 * @enum Sign types.
 */
typedef enum {
    SIGN_L,         // <
    SIGN_R,         // >
    SIGN_E,         // =
    SIGN_N          // None
} Sign_type;

/**
 * @enum Expression rules.
 */
typedef enum {
    RULE_ID,        // E -> id
    RULE_BR,        // E -> (E)
    RULE_ADD,       // E -> E + E
    RULE_SUB,       // E -> E - E
    RULE_DOT,       // E -> E . E
    RULE_MUL,       // E -> E * E
    RULE_DIV,       // E -> E / E
    RULE_CAR,       // E -> E ^ E
    RULE_EQ,        // E -> E === E
    RULE_NEQ,       // E -> E !== E
    RULE_LT,        // E -> E < E
    RULE_GT,        // E -> E > E
    RULE_LEQ,       // E -> E <= E
    RULE_GEQ,       // E -> E >= E
    RULE_N          // None
} Rule_type;

// Precedence table.
int64_t precedent_table[TAB_SIZE][TAB_SIZE] = 
{
    {SIGN_R, SIGN_L, SIGN_L, SIGN_R, SIGN_R, SIGN_L, SIGN_R, SIGN_L, SIGN_L},
    {SIGN_R, SIGN_R, SIGN_L, SIGN_R, SIGN_R, SIGN_L, SIGN_R, SIGN_L, SIGN_R},
    {SIGN_R, SIGN_R, SIGN_L, SIGN_R, SIGN_R, SIGN_L, SIGN_R, SIGN_L, SIGN_R},
    {SIGN_L, SIGN_L, SIGN_L, SIGN_R, SIGN_L, SIGN_L, SIGN_R, SIGN_L, SIGN_R},
    {SIGN_L, SIGN_L, SIGN_L, SIGN_R, SIGN_L, SIGN_L, SIGN_R, SIGN_L, SIGN_R},
    {SIGN_L, SIGN_L, SIGN_L, SIGN_L, SIGN_L, SIGN_L, SIGN_E, SIGN_L, SIGN_N},
    {SIGN_R, SIGN_R, SIGN_R, SIGN_R, SIGN_R, SIGN_N, SIGN_R, SIGN_N, SIGN_R},
    {SIGN_R, SIGN_R, SIGN_R, SIGN_R, SIGN_R, SIGN_N, SIGN_R, SIGN_N, SIGN_R},
    {SIGN_L, SIGN_L, SIGN_L, SIGN_L, SIGN_L, SIGN_L, SIGN_N, SIGN_L, SIGN_N}
};

/*
 * @struct Stack item representation.
 */
typedef struct symbol {
    Oper_type item;
    struct symbol *next;
} *symbolPtr;

/*
 * @struct Stack structure.
 */
typedef struct stack {
    symbolPtr top;
} symStack;

/**
 * Stack initialization.
 */
void stack_init(symStack*);

/**
 * Push a new element to stack.
 */
void stack_push(symStack*, Oper_type);

/**
 * Pop an element from stack.
 */
Oper_type stack_pop(symStack*);

/**
 *  Delete stack.
 */
void stack_clear(symStack*);

/*
 * Expression parsing.
 */
//int64_t expression_analysis();