#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <parser.h>


#define TAB_SIZE 8

/*
 * Expression parsing.
 */

void expression_parsing(ParserData* pd);

/**
 * @enum Operators of the precedence table.
 */
typedef enum {
    OPER_ADD,       // +
    OPER_SUB,       // - 
    OPER_DOT,       // .
    OPER_MUL,       // *
    OPER_DIV,       // /
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
    OPER_E,         // E
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

typedef enum {
    INDEX_0,
    INDEX_1,
    INDEX_2,
    INDEX_3,
    INDEX_4,
    INDEX_5,
    INDEX_6,
    INDEX_7
} Index_num;

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
    RULE_EQ,        // E -> E === E
    RULE_NEQ,       // E -> E !== E
    RULE_LT,        // E -> E < E
    RULE_GT,        // E -> E > E
    RULE_LEQ,       // E -> E <= E
    RULE_GEQ,       // E -> E >= E
    RULE_N          // None
} Rule_type;

extern int64_t precedent_table[TAB_SIZE][TAB_SIZE];
/*
 * @struct Stack element structure.
 */
typedef struct elementStack {
    Oper_type item;
    struct elementStack *next;
    //elementType?                                                              //FIX ME
} elementSymbol;

/*
 * @struct Stack structure.
 */
typedef struct {
    elementSymbol *top;
} stackSymbol;

/**
 * Stack initialization.
 */
void stack_init(stackSymbol* stack);

/**
 * Push a new element to stack.
 */
void stack_push(stackSymbol* stack, Oper_type);                                 // + DATA TYPE for GENERATE CODE?

/**
 * Pop an element from stack.
 */
void stack_pop(stackSymbol* stack);

/**
 *  Delete stack.
 */
void stack_clear(stackSymbol* stack);

/**
 * @brief Get stack top terminal.
 * 
 * @param stack 
 * @return elementSymbol 
 */
elementSymbol* term_top(stackSymbol* stack);

/**
 * @brief Get stack top nonterminal.
 * 
 * @param stack 
 * @return elementSymbol 
 */
elementSymbol* nonterm_top(stackSymbol* stack);

/**
 * @brief Get index for precedence table, get token type
 * 
 * @param token 
 * @return Oper_type 
 */
Oper_type term_info(Token* token);

Rule_type rule_info(elementSymbol* oper_third, elementSymbol* oper_second, elementSymbol* oper_first, int64_t cnt);