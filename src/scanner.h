#ifndef _SCANNER
#define _SCANNER

#include <stdio.h>
#include "string_t.h"

//FSM states.
#define STATE_START 100
#define STATE_ID_OR_KEYWORD 101                 // 'A..Z, a..z, 0..9, _'
#define STATE_NUMBER 102                        // '0..9'
#define STATE_NUMBER_POINT 103                  // '.'    P1
#define STATE_NUMBER_DOUBLE 104                 // '0..9'
#define STATE_NUMBER_EXPONENT_START 105         // 'E, e' P2
#define STATE_NUMBER_EXPONENT_SIGN 106          // '+, -' P3
#define STATE_NUMBER_EXPONENT 107               // '0..9'
#define STATE_STRING_START 108                  // '"'    P4
#define STATE_STRING_BACKSLASH 109              // '\'    P5 
#define STATE_STRING_BACKSLASH_ZERO_TO_ONE 110  // '0, 1' P6
#define STATE_STRING_BACKSLASH_TWO 111          // '2'    P7
#define STATE_STRING_BACKSLASH_ZERO_TO_NINE 112 // '0..9' P8 
#define STATE_STRING_BACKSLASH_FIVE 113         // '5'    P9
#define STATE_STRING 114                        // '", 0..5, 0..9'
#define STATE_MINUS 115                         // '-'
#define STATE_COMMENT 116                       // '--'   P10
#define STATE_COMMENT_BLOCK_START 117           // '['    P11
#define STATE_COMMENT_BLOCK_CONTINUE 118        // '[['   P12
#define STATE_COMMENT_BLOCK_END 119             // ']'    P13
#define STATE_CONCATENATION_START 120           // '.'    P14
#define STATE_CONCATENATION 121                 // '..'
#define STATE_DIVISION 122                      // '/'
#define STATE_DIVISION_INTEGER 123              // '//'
#define STATE_LESS 124                          // '<'
#define STATE_LESS_OR_EQUAL 125                 // '<='
#define STATE_GREATER 126                       // '>'
#define STATE_GREATER_OR_EQUAL 127              // '>='
#define STATE_EQUAL_SIGN 128                    // '='
#define STATE_IS_EQUAL 129                      // '=='
#define STATE_NOT_EQUAL_START 130               // '~'    P15
#define STATE_NOT_EQUAL 131                     // '~='
#define STATE_IS_EQUAL_END 132                  // '===' //!!
#define STATE_NOT_EQUAL_END 133                 // '!==' //!!

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
} Keywords;

//Enumeration of Token_types.
typedef enum {
    token_EOL=0,              // End of line.
    token_EOF,              // End of file.

    token_ID,               //Identificator.           
    token_keyword,          //Keyword.

    token_integer,          //Number (type integer).
    token_double,           //Number (type double).
    token_exponent,         //Number (type exponent).

    token_string,           //String.

    token_minus,            // "-"
    token_plus,             // "+"
    token_multiply,         // "*"
    token_divide,           // "/"
    token_divide_integer,   // "//"
    token_less,             // "<"
    token_less_or_equal,    // "<="
    token_greater,          // ">"
    token_greater_or_equal, // ">="
    token_equal_sign,       // "="
    token_equal,            // "=="
    token_not_equal,        // "~="
    token_left_bracket,     // "("
    token_right_bracket,    // ")"
    token_comma,            // ","
    token_colon,            // ":"
    token_length,           // "#"
    token_concatination,    // ".."

    token_question_mark,    // "?"
    token_semicolon,         // ";"
    token_left_curly_bracket, // "{"
    token_right_curly_bracket, // "}"
    token_point                // "."
} Token_types;

typedef union
{
    unsigned integer;      //Value for processing integer value.
    double   decimal;            //Value for processing double value.
    str_t*   String;        //Array for processing strings.
    Keywords keyword;          //Keyword.
} Token_value;


//Structure of token characteristics.
typedef struct {
    Token_value value;
    Token_types type; //Type of token.
} Token;

void scanner_init();
void scanner_reset();
void scanner_set_file(FILE* fptr);
//Comparing string we've gotten and compares with KW. In case it isn't a KW -> it's an ID.
bool determine_type(str_t *String, Token *Token);

//Gets a character from stdin and track location of lines and signs.
int getchar_modified();

//Function for reading string from stdin and converting into token.
int next_token(Token *token);

void scanner_terminate();

#endif