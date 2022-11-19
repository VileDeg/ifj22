#ifndef _SCANNER
#define _SCANNER

#include <stdio.h>
#include "string_t.h"

//FSM states.
#define STATE_START 100
#define STATE_ID_OR_KEYWORD 101                  // 'A..Z, a..z, 0..9, _'
#define STATE_NUMBER 102                         // '0..9'
#define STATE_NUMBER_POINT 103                   // '.'    P1
#define STATE_NUMBER_DOUBLE 104                  // '0..9'
#define STATE_NUMBER_EXPONENT_START 105          // 'E, e' P2
#define STATE_NUMBER_EXPONENT_SIGN 106           // '+, -' P3
#define STATE_NUMBER_EXPONENT 107                // '0..9'
#define STATE_STRING_START 108                   // '"'    P4
#define STATE_STRING_BACKSLASH 109               // '\'    P5 
#define STATE_STRING_BACKSLASH_ZERO 110          // '0'   
#define STATE_STRING_BACKSLASH_ZERO_ZERO 111     // '00'   
#define STATE_STRING_BACKSLASH_ONE_TWO_THREE 112 // '1,2,3' P6
#define STATE_STRING_BACKSLASH_ZERO_TO_SEVEN 113 // '0..7' P8 
#define STATE_STRING_BACKSLASH_HEX 115           // 'x'    
#define STATE_STRING_BACKSLASH_HEX_ZERO 116      // '0'   
#define STATE_STRING_BACKSLASH_HEX_FIRST 117     // '1..9, A..F, a..f'
#define STATE_STRING 118                         // '", 0..5, 0..9'
#define STATE_MINUS 119                          // '-'
#define STATE_COMMENT 120                        // '--'   P10
#define STATE_COMMENT_BLOCK_START 121            // '['    P11
#define STATE_COMMENT_BLOCK_CONTINUE 122         // '[['   P12
#define STATE_COMMENT_BLOCK_END 123              // ']'    P13
#define STATE_DIVISION 124                       // '/'
#define STATE_DIVISION_INTEGER 125               // '//'
#define STATE_LESS 126                           // '<'
#define STATE_LESS_OR_EQUAL 127                  // '<='
#define STATE_GREATER 128                        // '>'
#define STATE_GREATER_OR_EQUAL 129               // '>='
#define STATE_EQUAL_SIGN 130                     // '='
#define STATE_IS_EQUAL 131                       // '=='
#define STATE_NOT_EQUAL_START 132                // '~'    P15
#define STATE_NOT_EQUAL 133                      // '~='
#define STATE_IS_EQUAL_END 134                   // '===' 
#define STATE_NOT_EQUAL_END 135                  // '!==' 
#define STATE_QUESTION_MARK 136                  // '?'
#define STATE_PROLOG_ONE 137                     // '<?'
#define STATE_PROLOG_TWO 138                     // '<?p'
#define STATE_PROLOG_THREE 139                   // '<?ph'
#define STATE_PROLOG 140                         // '<?php'
#define STATE_END 141                            // '?>'

//Enumeration of keywords.
typedef enum {
    keyword_else=0,
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

//Enumeration of Token_types.
typedef enum {
    token_EOF = 0,             // End of file.
    token_ID,                  //Identificator.           
    token_keyword,             //Keyword.
    token_integer,             // Number (type integer).
    token_float,               // Number (type float).
    //token_exponent,            // Number (type exponent).
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
    token_prologue,              // "<?php"
    token_dollar,              // "$"
    token_end                  // "?<"
} Token_type;

//Structure of token characteristics.
typedef struct 
{
    unsigned integer;      //Value for processing integer value.
    double   decimal;      //Value for processing double value.
    str_t    string ;      //Array for processing strings.
    Keyword  keyword;      //Keyword.
    
    Token_type type; //Type of token.
    bool questionmark;
} Token;



void token_clear(Token *tk);

void token_dest(Token* tk);

bool token_const(Token* tk);

bool token_cpy(Token* dst, Token* src);

void scanner_set_file(FILE* fptr);

FILE* scanner_get_file();

void scanner_set_string(str_t* str);

//Function for reading string from stdin and converting into token.
int scanner_get_next_token(Token *token);

#endif