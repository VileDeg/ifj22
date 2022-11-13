#ifndef __PRECEDENCE_T__
#define __PRECEDENCE_T__

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "parser.h"




#define TAB_SIZE 8

/*
 * Expression parsing.
 */
int64_t expression_parsing(ParserData* pd);

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

    DATA_ID,        // id
    DATA_INT,       // int
    DATA_FLOAT,     // float
    DATA_STRING,    // string
    
    OPER_DOLLAR,    // $
    OPER_REDUCE,   // reduce (Must be < OPER_EXPR !!)
    OPER_E       // E
} Oper_type;

/**
 * @enum Sign types.
 */
typedef enum {
    SHIFT,         // <
    REDUCE,         // >
    EQUAL,         // =
    NONE          // None
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



#endif //__PRECEDENCE_T__