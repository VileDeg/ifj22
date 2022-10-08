//#include "base.h"

#include "scanner.h"
#include "errors.h"
#include <stdlib.h>
#include <ctype.h>          

#define END_OF_FILE 42

// File to read from
static FILE* s_fptr;

// Creates dynamic string.
static str_t* String;

static unsigned int sign_counter = 0;
static unsigned int line_counter = 1;

void scanner_set_file(FILE* fptr)
{
    s_fptr = fptr;
}

FILE* scanner_get_file()
{
    return s_fptr;
}

void scanner_set_string(str_t* str)
{
    String = str;
}

// void scanner_init()
// {
//     s_fptr = stdin;
//     String = calloc(1, sizeof(*String));
//     str_const(String);
// }

void scanner_reset()
{
    sign_counter = 0;
    line_counter = 1;
}

void scanner_free()
{
    str_dest(String);
    free(String);
}

//Comparing string we've gotten and compares with KW. In case it isn't a KW -> it's an ID.
bool determine_type(str_t *String, Token *Token)
{
    //Whether an identifier has been set.
    bool flag_id = false;
    
    if (str_cmp(String, "else"))
        Token->value.keyword = keyword_else;
    else if (str_cmp(String, "float"))
        Token->value.keyword = keyword_float;  
    else if (str_cmp(String, "function"))
        Token->value.keyword = keyword_function;
    else if (str_cmp(String, "if"))
        Token->value.keyword = keyword_if;
    else if (str_cmp(String, "int"))
        Token->value.keyword = keyword_int;
    else if (str_cmp(String, "null"))
        Token->value.keyword = keyword_null;
    else if (str_cmp(String, "return"))
        Token->value.keyword = keyword_return;
    else if (str_cmp(String, "string"))
        Token->value.keyword = keyword_string;
    else if (str_cmp(String, "void"))
        Token->value.keyword = keyword_void;
    else if (str_cmp(String, "while"))
        Token->value.keyword = keyword_while;
    else
    {
        Token->type = token_ID;
        flag_id = true;
    } 

    if (flag_id == false)
        Token->type = token_keyword;

    return 0; //Scanner token was successful.
}

//Gets a character from stdin and track location of lines and signs.
int getchar_modified()
{
    int term_sign = getc(s_fptr);

    if (term_sign != '\n')
        sign_counter++;
    else
    {
        sign_counter = 0;
        line_counter++;
    }
    
    return term_sign;
}



//Function for reading string from stdin and converting into token.
int scanner_get_next_token(Token *Token)
{
    str_clear(String);
    Token->value.String = String;
    
    int current_state = STATE_START; 
    int sign;                 //Sign which is taken one by one from the input string.
    bool is_sign = false;     //For STATE_NUMBER case.
    bool is_exponent = false; //For STATE_NUMBER_DOUBLE case.
    char ascii_oct[3];        //For working with octal numbers in escape sequences.
    char ascii_hex[2];        //For working with hex numbers in escape sequences.
    
    while(true)
    {
        sign = getchar_modified();

        switch(current_state)
        {
            //Handling a situation with a coincidence of signs.
            case(STATE_START):

                // if (sign == '\n')
                // {
                //     Token->type = token_EOL;
                //     return 0;
                // }

                if (sign == '_' || isalpha(sign) || sign == '$')
                {
                    str_add_sign(String, sign);
                    current_state = STATE_ID_OR_KEYWORD; 
                    break;
                }

                // if (sign == '$')
                // {
                //     current_state = STATE_ID_OR_KEYWORD;
                //     break;
                // }
                

                if (isdigit(sign))
                {
                    str_add_sign(String, sign);
                    current_state = STATE_NUMBER;
                    break;
                }


                if (sign == '+')
                {
                    Token->type = token_plus;
                    
                    return 0;
                }

                if (sign == '-')
                {
                    Token->type = token_minus;
                    
                    return 0;
                }


                if (sign == '*')
                {
                    Token->type = token_multiply;
                    
                    return 0;
                }
                

                if (sign == '(')
                {
                    Token->type = token_left_bracket;
                    
                    return 0;
                }


                if (sign == ')')
                {
                    Token->type = token_right_bracket;
                    
                    return 0;
                }


                if (sign == ',')
                {
                    Token->type = token_comma;
                    
                    return 0;
                }


                if (sign == ':')
                {
                    Token->type = token_colon;
                    
                    return 0;
                }
            

                if (sign == ';')
                {
                    Token->type = token_semicolon;

                    return 0;
                }
                    
                if (sign == '{')
                {
                    Token->type = token_left_curly_bracket;

                    return 0;
                }

                if (sign == '}')
                {
                    Token->type = token_right_curly_bracket;

                    return 0;
                }

                if (sign == '.')
                {
                    Token->type = token_dot;

                    return 0;
                }
                
                if (sign == '?')
                {  
                    current_state = STATE_QUESTION_MARK;
                    break;
                }

                if (sign == '!')
                {
                    current_state = STATE_NOT_EQUAL_START;
                    break;
                }

                if (sign == '/')
                {
                    current_state = STATE_DIVISION;
                    break;
                }


                if (sign == '<')
                {
                    current_state = STATE_LESS;
                    break;
                }
                

                if (sign == '>')
                {
                    current_state = STATE_GREATER;
                    break;
                }


                if (sign == '=')
                {
                    current_state = STATE_EQUAL_SIGN;
                    break;
                }


                if (sign == '\"')
                {
                    current_state = STATE_STRING_START;
                    break;
                }
                
                
                if (isspace(sign) || sign == '\t')
                    break;


                if (sign == EOF)
                {
                    Token->type = token_EOF;
                    
                    return END_OF_FILE;
                }

                //If there were no signs matches, then a lexical error is written out.
                //PRINT_ERROR("[LEXICAL ERROR]:%d:%d: invalid sign: \"%c\"\n", line_counter, sign_counter, sign);
                PRINT_ERROR_LEX("%d:%d: invalid sign: \"%c\"", line_counter, sign_counter, sign);
                // fprintf(stderr, "\033[1;31m");  //!! Color of the error message.
                // fprintf(stderr, "[LEXICAL ERROR]:%d:%d: invalid sign: \"%c\"\n", line_counter, sign_counter, sign);
                // fprintf(stderr, "\033[0m");

                return ERROR_LEXICAL;

            case(STATE_PROLOG_ONE):
                if (sign == 'p')
                {
                    current_state = STATE_PROLOG_TWO;
                }
                else
                {
                    //Error handling. 
                    str_add_sign(String, sign);
                    PRINT_ERROR_LEX("wrong prolog");
                    
                    return ERROR_LEXICAL; 
                }
                break;
            
            case(STATE_PROLOG_TWO):
                if (sign == 'h')
                {
                    current_state = STATE_PROLOG_THREE;
                }
                else
                {
                    //Error handling. 
                    str_add_sign(String, sign);
                    PRINT_ERROR_LEX("wrong prolog");
                    
                    return ERROR_LEXICAL; 
                }
                break;

            case(STATE_PROLOG_THREE):
                if (sign == 'p')
                {
                    current_state = STATE_PROLOG;
                }
                else
                {
                    //Error handling. 
                    str_add_sign(String, sign);
                    PRINT_ERROR_LEX("wrong prolog");
                    
                    return ERROR_LEXICAL; 
                }
                break;

            case(STATE_PROLOG):
                Token->type = token_prolog;
                return 0;
                                    
            case(STATE_QUESTION_MARK):
                if (sign == '>')
                {
                    current_state = STATE_END;
                }
                else if (sign == 'f' || sign == 'i' || sign == 's')
                {
                    current_state = STATE_ID_OR_KEYWORD;
                    str_add_sign(String, sign);
                    break;
                }
                else
                {
                    //Error handling. 
                    str_add_sign(String, sign);
                    PRINT_ERROR_LEX("wrong character after '?'");
                    
                    return ERROR_LEXICAL; 
                }

                break;

            case(STATE_END):

                //проверка следующего символа(его не должно быть)
                if (sign == EOF)
                {
                    Token->type = token_end;
                    ungetc(sign, s_fptr);
                }
                else
                {
                    //Error handling. 
                    str_add_sign(String, sign);
                    PRINT_ERROR_LEX("Character after the closing character '?>'");
                    
                    return ERROR_LEXICAL; 
                }
                
                return 0;


            
            //Handle a word and sorting it by ID or keyword.
            case(STATE_ID_OR_KEYWORD):
                while(true)
                {
                    if (isalpha(sign) || isdigit(sign) || sign == '_')
                        str_add_sign(String, sign);
                    else
                        break;
                    sign = getchar_modified();
                }
                determine_type(String, Token); 
                //Returns the sign back to stream. 
                ungetc(sign, s_fptr);

                return 0;

            //Handle a number and sorting it by integer, decimal or exponent.
            case(STATE_NUMBER):
                while(true)
                {
                    if (isdigit(sign))
                        str_add_sign(String, sign);
                    else if ( sign == '.')
                    {
                        str_add_sign(String, sign);
                        is_sign = true;
                        current_state = STATE_NUMBER_POINT;
                        break;
                    }
                    else if (sign == 'E' || sign == 'e')
                    {
                        str_add_sign(String, sign);
                        current_state = STATE_NUMBER_EXPONENT_START;
                        is_sign = true;
                        break;
                    }
                    else if (isspace(sign) || sign == '\n' || sign == EOF || sign == '\t')
                        break;
                    else if (sign == '-' || sign == '+' || sign == '*' || sign == '/' || sign == '<' || sign == '>' || 
                             sign == '=' || sign == '~' || sign == '(' || sign == ')' || sign == ',' || sign == ';')
                        break;
                    else if (!isdigit(sign))
                    {
                        //Error handling. 
                        str_add_sign(String, sign);
                        PRINT_ERROR_LEX("%d:%d: wrong form of integer: \"%s\"", line_counter, sign_counter, Token->value.String->ptr);
                        
                        return ERROR_LEXICAL; 
                    }
                    
                    sign = getchar_modified();
                }    

                if (is_sign == true)
                    break;

                //Converts string to int. 
                Token->value.integer = atoi(String->ptr);
                Token->type = token_integer;
                current_state = STATE_START;
                ungetc(sign, s_fptr);
                
                return 0;

            //Handle an exponent.
            case(STATE_NUMBER_EXPONENT_START):
                if ( sign == '+' || sign == '-' )
                {
                    str_add_sign(String, sign);
                    current_state = STATE_NUMBER_EXPONENT_SIGN;
                    break;
                } 
                else if ( isdigit(sign) )
                {
                    str_add_sign(String, sign);
                    current_state = STATE_NUMBER_EXPONENT;
                    break;
                }
                else if (isspace(sign) || sign == '\n' || sign == EOF || sign == '\t')
                {
                    current_state = STATE_NUMBER_EXPONENT;
                    ungetc(sign, s_fptr);
                    break;
                }
                else 
                {
                    str_add_sign(String, sign);
                    PRINT_ERROR_LEX("%d:%d: wrong form of exponent: \"%s\"", line_counter, sign_counter, Token->value.String->ptr);
                    
                    return ERROR_LEXICAL;
                }

            //Handle an exponent using plus or minus.
            case(STATE_NUMBER_EXPONENT_SIGN):
                if ( isdigit(sign) )
                {
                    str_add_sign(String, sign);
                    current_state = STATE_NUMBER_EXPONENT;
                    break;
                } 
                else if (isspace(sign) || sign == '\n' || sign == EOF || sign == '\t')
                {
                    current_state = STATE_NUMBER_EXPONENT;
                    ungetc(sign, s_fptr);
                    break;
                }
                else
                {
                    str_add_sign(String, sign);
                    PRINT_ERROR_LEX("%d:%d: wrong form of exponent: \"%s\"", line_counter, sign_counter, Token->value.String->ptr);
                    
                    return ERROR_LEXICAL;
                }
            
            //The last stage of processing the exponent.
            case(STATE_NUMBER_EXPONENT):
                while(true)
                {
                    if (isdigit(sign))
                        str_add_sign(String, sign);
                    else if (isspace(sign) || sign == '\n' || sign == EOF || sign == '\t')
                        break;
                    else
                    {
                        str_add_sign(String, sign);
                        PRINT_ERROR_LEX("%d:%d: wrong form of exponent: \"%s\"", line_counter, sign_counter, Token->value.String->ptr);
                        
                        return ERROR_LEXICAL; 
                    }
                         
                    sign = getchar_modified();                      
                }
                Token->type = token_exponent;
                //Converts string to a floating-point number (decimal).
                Token->value.decimal = atof(String->ptr);
                ungetc(sign, s_fptr);
                
                return 0;

            //First handle a floating-point number.
            case(STATE_NUMBER_POINT):

                if (isdigit(sign)) 
                {
                    str_add_sign(String, sign);
                    current_state = STATE_NUMBER_DOUBLE;
                }
                else
                {
                    str_add_sign(String, sign);
                    PRINT_ERROR_LEX("%d:%d: wrong form of decimal: \"%s\"", line_counter, sign_counter, Token->value.String->ptr);
                    
                    return ERROR_LEXICAL; 
                }
                break;

            //Second handle a floating-point number.
            case(STATE_NUMBER_DOUBLE):
            while(true)
            {
                if (isdigit(sign))
                    str_add_sign(String, sign);
                else if (sign == 'E' || sign == 'e')
                {
                    str_add_sign(String, sign);
                    current_state = STATE_NUMBER_EXPONENT_START;
                    is_exponent = true;
                    break;
                }
                else if (isspace(sign) || sign == '\n' || sign == EOF || sign == '\t')
                    break;
                else if (sign == '-' || sign == '+' || sign == '*' || sign == '/' || sign == '<' || sign == '>' || 
                         sign == '=' || sign == '~' || sign == '(' || sign == ')' || sign == ',')
                    break;
                else if (!isdigit(sign))
                {
                    str_add_sign(String, sign);
                    PRINT_ERROR_LEX("%d:%d: wrong form of decimal: \"%s\"", line_counter, sign_counter, Token->value.String->ptr);
                    
                    return ERROR_LEXICAL; 
                }
                
                sign = getchar_modified();
            }        
            if (is_exponent == true) break;
            Token->value.decimal = atof(String->ptr);
            Token->type = token_float;

            current_state = STATE_START;
            ungetc(sign, s_fptr);
            
            return 0;
/*    
            //Handling a situation with division sign '/'.
            case(STATE_DIVISION):
                if (sign == '/')
                    current_state = STATE_DIVISION_INTEGER;
                else
                    {
                        ungetc(sign, s_fptr);
                        Token->type = token_divide;
                        
                        return 0;
                    }
                break;

            //Handling a situation with division integer "//".
            case(STATE_DIVISION_INTEGER):
                Token->type = token_divide_integer;
                ungetc(sign, s_fptr);
                
                return 0;

*/
            
            //Handling a situation with relation operator '<'.
            case(STATE_LESS):
                if (sign == '=')
                    current_state = STATE_LESS_OR_EQUAL;
                else if (sign == '?')
                {
                    current_state = STATE_PROLOG_ONE;
                }
                else
                {
                    ungetc(sign, s_fptr);
                    Token->type = token_less;
                    
                    return 0;
                }
                break;
            
            //Handling a situation with relation operator "<=".
            case(STATE_LESS_OR_EQUAL):
                Token->type = token_less_or_equal;
                ungetc(sign, s_fptr);
                
                return 0;

            
            //Handling a situation with relation operator '>'.
            case(STATE_GREATER):
                if (sign == '=')
                    current_state = STATE_GREATER_OR_EQUAL;
                else
                    {
                        ungetc(sign, s_fptr);
                        Token->type = token_greater;
                        
                        return 0;
                    }
                break;

            //Handling a situation with relation operator ">=".
            case(STATE_GREATER_OR_EQUAL):
                Token->type = token_greater_or_equal;
                ungetc(sign, s_fptr);
                
                return 0;


            //Handling a situation with equal sign '='.
            case(STATE_EQUAL_SIGN):
                if (sign == '=')
                    current_state = STATE_IS_EQUAL;
                else
                {
                    ungetc(sign, s_fptr);
                    Token->type = token_equal_sign; 
                    
                    return 0;
                }
                break;
            
            //Handling a situation with relation operator "==". 
            case(STATE_IS_EQUAL):
                if(sign == '=')
                    current_state = STATE_IS_EQUAL_END;
                else
                {
                    //Error handling. 
                    str_add_sign(String, sign);
                    PRINT_ERROR_LEX("%d:%d: wrong сharacter \"%s\"", line_counter, sign_counter, Token->value.String->ptr);
                    
                    return ERROR_LEXICAL; 
                }
                
                break;

            //Handling a situation with relation operator '==='.
            case(STATE_IS_EQUAL_END):
                Token->type = token_equal; 
                ungetc(sign, s_fptr);

                return 0;

            
            //Handling a situation with start of relation operator "!=".
            case(STATE_NOT_EQUAL_START):
                if (sign == '=')
                    current_state = STATE_NOT_EQUAL;
                else
                {
                    str_add_sign(String,'!');
                    str_add_sign(String,sign);
                    PRINT_ERROR_LEX("%d:%d: unsuitable combination of characters: \"%s\"", line_counter, sign_counter, Token->value.String->ptr);
                    
                    return ERROR_LEXICAL;
                }
                break;
            
            //Handling a situation with relation operator "!=".
            case(STATE_NOT_EQUAL):
                if(sign == '=')
                     current_state = STATE_NOT_EQUAL_END;
                else
                {
                    str_add_sign(String,'!');
                    str_add_sign(String,sign);
                    PRINT_ERROR_LEX("%d:%d: unsuitable combination of characters: \"%s\"", line_counter, sign_counter, Token->value.String->ptr);
                    
                    return ERROR_LEXICAL;
                }
                break;

            //Handling a situation with relation operator "!==".
            case(STATE_NOT_EQUAL_END):    
                Token->type = token_not_equal;
                ungetc(sign, s_fptr);
                
                return 0;

            //Handling a situation with minus sign '/'.    
            case(STATE_DIVISION):
                if (sign == '/') //значит это однострочный комментарий 
                    current_state = STATE_COMMENT;
                else if (sign == '*') //значит это многострочный комментарий
                    current_state = STATE_COMMENT_BLOCK_START;
                else
                {
                    ungetc(sign, s_fptr);
                    Token->type = token_divide;
                    
                    return 0; 
                }
                break;

            //Handling a situation with line comment "//".
            case(STATE_COMMENT):
                // if (sign == '[')
                // {
                //     current_state = STATE_COMMENT_BLOCK_START;
                //     break;
                // }
                while(true)
                {
                    if (sign == EOF || sign == '\n')
                    {
                        ungetc(sign, s_fptr);
                        break;
                    }
                    else if (sign != '\n')
                        sign = getchar_modified();
                    else 
                        break;
                }
                line_counter--;
                current_state = STATE_START;
                break;

            //Handling a situation with start of block comment "/*".
            case(STATE_COMMENT_BLOCK_START): 
                while(true)
                {
                    if (sign == EOF)
                    {
                        current_state = STATE_START;
                        ungetc(sign, s_fptr);
                        break;
                    }
                    else if (sign != '*') 
                        sign = getchar_modified();
                    else
                    {
                        current_state = STATE_COMMENT_BLOCK_END;
                        break;
                    }
                }
                break;

            //Handling a situation with end of block comment "/**/". 
            case(STATE_COMMENT_BLOCK_END):
                if (sign == '/')
                    current_state = STATE_START;
                else 
                {
                    current_state = STATE_COMMENT_BLOCK_CONTINUE;
                    ungetc(sign, s_fptr);
                }
                break;

            //Handling a situation with start of string '"'.
            case(STATE_STRING_START):
                while(true)
                {
                    if (sign == '\"')
                    {
                        current_state = STATE_STRING;
                        break;
                    }
                    else if (sign == '\\')
                    {
                        current_state = STATE_STRING_BACKSLASH;
                        break;
                    }
                    else if (sign == '\n' || sign == EOF)
                    {
                        PRINT_ERROR_LEX("%d:%d: no end of string", line_counter, sign_counter);
                        
                        return ERROR_LEXICAL;
                    }
                    str_add_sign(String, sign);
                    sign = getchar_modified();
                }
                break;
            
            //Handling a situation with escape sequence in string '\'.
            case(STATE_STRING_BACKSLASH):
                if (sign == '\"')
                {
                    current_state = STATE_STRING_START;
                    str_add_sign(String, sign);
                    break;
                }
                else if (sign == 'n')
                {
                    current_state = STATE_STRING_START;
                    str_add_sign(String, '\n'); 
                    break;
                }
                else if (sign == 't')
                {
                    current_state = STATE_STRING_START;
                    str_add_sign(String, '\t');
                    break;
                }
                else if (sign == '\\')
                {
                    current_state = STATE_STRING_START;
                    str_add_sign(String, '\\');
                    break;
                }
                else if (sign == '$')
                {
                    current_state = STATE_STRING_START;
                    str_add_sign(String, '$');    
                    break;
                }
                else if (sign == '0')
                {
                    current_state = STATE_STRING_BACKSLASH_ZERO;
                    ascii_oct[0] = sign; 
                    break;
                }
                else if (sign == '1' || sign == '2' || sign == '3')
                {
                    current_state = STATE_STRING_BACKSLASH_ONE_TWO_THREE;
                    ascii_oct[0] = sign; 
                    break;
                }
                else if (sign == 'x')
                {
                    current_state = STATE_STRING_BACKSLASH_HEX;
                    break;
                }    
                else 
                {
                    str_add_sign(String, '\\');
                    str_add_sign(String, sign);
                    PRINT_ERROR_LEX("%d:%d: invalid escape sequence: \"%s\"", line_counter, sign_counter, Token->value.String->ptr);
                    
                    return ERROR_LEXICAL;
                }


            //Handling a situation with escape sequence ascii[d][][].
            case(STATE_STRING_BACKSLASH_ZERO):
                if (sign >= '1' && sign <= '7')
                {
                    current_state = STATE_STRING_BACKSLASH_ZERO_TO_SEVEN;
                    ascii_oct[1] = sign;
                    break;
                }
                else if (sign == '0')
                {
                    current_state = STATE_STRING_BACKSLASH_ZERO_ZERO;
                    ascii_oct[1] = sign;
                    break;
                }
                else
                {
                    PRINT_ERROR_LEX("%d:%d: invalid escape sequence", line_counter, sign_counter);

                    return ERROR_LEXICAL;
                }

            case(STATE_STRING_BACKSLASH_ZERO_ZERO):
                if (sign >= '1' && sign <= '7')
                {
                    //[d][d][d].
                    current_state = STATE_STRING_START;
                    ascii_oct[2] = sign;
                    //str_add_sign(String, atoi(ascii_oct));
                    str_add_sign(String, strtol(ascii_oct, NULL, 8));
                    break;
                }
                else
                {
                    PRINT_ERROR_LEX("%d:%d: invalid escape sequence", line_counter, sign_counter);

                    return ERROR_LEXICAL;
                }


            //Handling a situation with escape sequence ascii[d][][].
            case(STATE_STRING_BACKSLASH_ONE_TWO_THREE):
                if (sign >= '0' && sign <= '7')
                {
                    current_state = STATE_STRING_BACKSLASH_ZERO_TO_SEVEN;
                    ascii_oct[1] = sign;
                    break;
                }
                else 
                {
                    PRINT_ERROR_LEX("%d:%d: invalid escape sequence", line_counter, sign_counter);
                    
                    return ERROR_LEXICAL;
                }
                    

            //Handling a situation with escape sequence ascii[d][d][].
            case(STATE_STRING_BACKSLASH_ZERO_TO_SEVEN):
                if (sign >= '0' && sign <= '7')
                {   
                    //[d][d][d].
                    current_state = STATE_STRING_START;
                    ascii_oct[2] = sign;
                    //str_add_sign(String, atoi(ascii_oct));
                    str_add_sign(String, strtol(ascii_oct, NULL, 8));
                    break;
                }
                else 
                {
                    PRINT_ERROR_LEX("%d:%d: invalid escape sequence", line_counter, sign_counter);
                    
                    return ERROR_LEXICAL;
                }


            //Handling a situation with escape sequence ascii[d][].
            case(STATE_STRING_BACKSLASH_HEX):   
                
                if (sign == '0')
                {
                    current_state = STATE_STRING_BACKSLASH_HEX_ZERO;;
                    ascii_hex[0] = sign;
                    break;
                }
                else if (sign >= '1' && sign <= '9' || sign >= 'a' && sign <= 'f' || sign >= 'A' && sign <= 'F')
                {
                    current_state = STATE_STRING_BACKSLASH_HEX_FIRST;
                    ascii_hex[0] = sign;
                    break;
                }
                else 
                {
                    PRINT_ERROR_LEX("%d:%d: invalid escape sequence", line_counter, sign_counter);
                    
                    return ERROR_LEXICAL;
                }

            //Handling a situation with escape sequence ascii[d][d].
            case(STATE_STRING_BACKSLASH_HEX_FIRST):
                if (sign >= '0' && sign <= '9' || sign >= 'a' && sign <= 'f' || sign >= 'A' && sign <= 'F')
                {
                    current_state = STATE_STRING_START;
                    ascii_hex[1] = sign;
                    str_add_sign(String, strtol(ascii_hex, NULL, 16));

                    break;
                }
                else 
                {
                    PRINT_ERROR_LEX("%d:%d: invalid escape sequence", line_counter, sign_counter);
                    
                    return ERROR_LEXICAL;
                }

            case(STATE_STRING_BACKSLASH_HEX_ZERO):
                if (sign >= '1' && sign <= '9' || sign >= 'a' && sign <= 'f' || sign >= 'A' && sign <= 'F')
                {
                    current_state = STATE_STRING_START;
                    ascii_hex[1] = sign;
                    str_add_sign(String, strtol(ascii_hex, NULL, 16));

                    break;
                }
                else 
                {
                    PRINT_ERROR_LEX("%d:%d: invalid escape sequence", line_counter, sign_counter);
                    
                    return ERROR_LEXICAL;
                }


            //Handling a situation with end of string '"'.
            case(STATE_STRING):
                Token->type = token_string;
                current_state = STATE_START;
                ungetc(sign, s_fptr);
                
                return 0;

        }
    }
}