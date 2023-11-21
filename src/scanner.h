/**
 * Project: Implementation of imperative language IFJ22 compiler.
 * 
 * @author Vladyslav Kovalets (xkoval21@stud.fit.vutbr.cz)
 * @author Vadim Goncearenco  (xgonce00@stud.fit.vutbr.cz)
 */

#ifndef _SCANNER
#define _SCANNER

#include <stdio.h>

#include "string_t.h"

/* Finite state machine states. */ 
#define STATE_START 100
#define STATE_ID_OR_KEYWORD 101
#define STATE_NUMBER 102
#define STATE_NUMBER_POINT 103
#define STATE_NUMBER_DOUBLE 104
#define STATE_NUMBER_EXPONENT_START 105
#define STATE_NUMBER_EXPONENT_SIGN 106
#define STATE_NUMBER_EXPONENT 107
#define STATE_STRING_START 108
#define STATE_STRING_BACKSLASH 109
#define STATE_STRING_BACKSLASH_ZERO 110
#define STATE_STRING_BACKSLASH_ZERO_ZERO 111
#define STATE_STRING_BACKSLASH_ONE_TWO_THREE 112
#define STATE_STRING_BACKSLASH_ZERO_TO_SEVEN 113
#define STATE_STRING_BACKSLASH_HEX 114
#define STATE_STRING_BACKSLASH_HEX_ZERO 115
#define STATE_STRING_BACKSLASH_HEX_FIRST 116
#define STATE_STRING 117
#define STATE_COMMENT 118
#define STATE_COMMENT_BLOCK_START 119
#define STATE_COMMENT_BLOCK_END 120
#define STATE_DIVISION 121
#define STATE_LESS 122
#define STATE_LESS_OR_EQUAL 123
#define STATE_GREATER 124
#define STATE_GREATER_OR_EQUAL 125
#define STATE_EQUAL_SIGN 126
#define STATE_IS_EQUAL 127
#define STATE_NOT_EQUAL_START 128
#define STATE_NOT_EQUAL 129
#define STATE_IS_EQUAL_END 130
#define STATE_NOT_EQUAL_END 131
#define STATE_QUESTION_MARK 132
#define STATE_PROLOG_ONE 133
#define STATE_PROLOG_TWO 134
#define STATE_PROLOG_THREE 135
#define STATE_PROLOG 136
#define STATE_END 137

/* Enumeration of keywords. */
typedef enum {
    keyword_else = 0,
    keyword_float,
    keyword_function,
    keyword_if,
    keyword_int,
    keyword_null,
    keyword_return,
    keyword_string,
    keyword_void,
    keyword_while
} Keyword;

/* Enumeration of Token_types. */
typedef enum {
    token_EOF = 0,             // End of file.
    token_ID,                  // Identificator.           
    token_keyword,             // Keyword.
    token_integer,             // Number (type integer).
    token_float,               // Number (type float).
    token_string,              // String.
    token_null,                // null
    token_minus,               // "-"
    token_plus,                // "+"
    token_multiply,            // "*"
    token_divide,              // "/"
    token_less,                // "<"
    token_less_or_equal,       // "<="
    token_greater,             // ">"
    token_greater_or_equal,    // ">="
    token_equal_sign,          // "="
    token_equal,               // "==="
    token_not_equal,           // "!=="
    token_left_bracket,        // "("
    token_right_bracket,       // ")"
    token_comma,               // ","
    token_colon,               // ":"
    token_semicolon,           // ";"
    token_left_curly_bracket,  // "{"
    token_right_curly_bracket, // "}"
    token_dot,                 // "."
    token_prologue,            // "<?php"
    token_dollar,              // "$"
    token_end                  // "?<"
} Token_type;

/**
 * @struct Struct with token attributes
 */
typedef struct 
{
    unsigned integer;      //Value for processing integer value.
    double   decimal;      //Value for processing double value.
    str_t    string ;      //Array for processing strings.
    Keyword  keyword;      //Keyword.
    
    Token_type type; //Type of token.
    bool questionmark;
} Token;


/**
 * Clears token contents.
 * @param tk token
 */
void token_clear(Token *tk);

/**
 * Destructs token.
 * @param tk token
 */
void token_dest(Token* tk);

/**
 * Constructs token.
 * @param tk token
 * @return True if success.
 */
bool token_const(Token* tk);

/**
 * Creates deep copy of token.
 * @param dst destination
 * @param src source
 * @return True if success.
 */
bool token_cpy(Token* dst, Token* src);

/**
 * Sets input file for scanner.
 * @param fptr file pointer
 */
void scanner_set_file(FILE* fptr);

/**
 * Gets next token from input stream.
 * @param tk token
 * @return Error code.
 */
int scanner_get_next_token(Token* tk);

#endif