/**
 * Project: Implementation of imperative language IFJ22 compiler.
 * 
 * @author Vladyslav Kovalets (xkoval21@stud.fit.vutbr.cz)
 * @author Vadim Goncearenco  (xgonce00@stud.fit.vutbr.cz)
 */

#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "scanner.h"
#include "errors.h"
#include "parser.h"


static FILE* s_Fptr; // File to read from.

static unsigned int sign_counter = 0; 
static unsigned int line_counter = 1; 
bool is_correct_file_start = false;

void scanner_set_file(FILE* fptr)
{
    s_Fptr = fptr;
}


/**
 * Checks if string is keyword.
 * @param str string
*/
bool determine_type(Token* tk)
{
    bool flag_id = false;
    bool flag_null = false;

    if (str_cmp(&tk->string, "else"))
        tk->keyword = keyword_else;
    else if (str_cmp(&tk->string, "float"))
        tk->keyword = keyword_float;  
    else if (str_cmp(&tk->string, "int"))
        tk->keyword = keyword_int;
    else if (str_cmp(&tk->string, "string"))
        tk->keyword = keyword_string;
    else if (str_cmp(&tk->string, "function"))
        tk->keyword = keyword_function;
    else if (str_cmp(&tk->string, "if"))
        tk->keyword = keyword_if;
    else if (str_cmp(&tk->string, "return"))
        tk->keyword = keyword_return;
    else if (str_cmp(&tk->string, "void"))
        tk->keyword = keyword_void;
    else if (str_cmp(&tk->string, "while"))
        tk->keyword = keyword_while;
    else if (str_cmp(&tk->string, "null"))
    {
        tk->type = token_null;
        tk->keyword = keyword_null;
        flag_null = true;
    }
    else
    {
        tk->type = token_ID;
        flag_id = true;
    } 

    if (!flag_id && !flag_null)
        tk->type = token_keyword;

    return 0; //Scanner tk was successful.
}


/**
 * Gets a character from stdin and track location of lines and signs.
 * @return character
*/
int getchar_modified()
{
    int term_sign = getc(s_Fptr);

    if (term_sign != '\n')
        sign_counter++;
    else
    {
        sign_counter = 0;
        line_counter++;
    }
    
    return term_sign;
}


void token_clear(Token *tk)
{
    str_clear(&tk->string); 
    tk->integer = tk->type = tk->questionmark = 0; 
}


void token_dest(Token* tk)
{
    str_dest(&tk->string); 
}


bool token_const(Token* tk)
{
    memset(tk, 0, sizeof(Token));
    if (!str_const(&tk->string))
        return false;
    return true;
}


bool token_cpy(Token* dst, Token* src) 
{
    *dst = *src;
    if (!str_const(&dst->string))
        return false;
    str_cpy(&dst->string, &src->string);
    return true;
}


int scanner_get_next_token(Token* tk)
{
    if (!token_const(tk))
        INTERNAL_ERROR_RET("Memory fail.");
    
    int current_state = STATE_START; 
    int sign;                 // Character which is taken one by one from the input string.
    bool is_sign = false;     // For STATE_NUMBER case.
    bool is_exponent = false; // For STATE_NUMBER_DOUBLE case.
    char ascii_oct[3];        // For working with octal numbers in escape sequences.
    char ascii_hex[2];        // For working with hex numbers in escape sequences.
    
    while(true) 
    {
        sign = getchar_modified(); 
        
        if(is_correct_file_start == false)
        {
            if (sign != '<')
            {
                PRINT_ERROR_RET(ERROR_LEXICAL, "wrong file start");
            }
                
            is_correct_file_start = true;
        }

        switch(current_state)
        {

            /* Character processing. */
            case(STATE_START):
  
                if (sign == '_' || isalpha(sign) || sign == '$')
                {
                    str_add_sign(&tk->string, sign);
                    current_state = STATE_ID_OR_KEYWORD; 
                    break;
                }
                

                if (isdigit(sign))
                {
                    str_add_sign(&tk->string, sign);
                    current_state = STATE_NUMBER;
                    break;
                }


                if (sign == '+')
                {
                    tk->type = token_plus;
                    
                    return 0;
                }


                if (sign == '-')
                {
                    tk->type = token_minus;
                    
                    return 0;
                }


                if (sign == '*')
                {
                    tk->type = token_multiply;
                    
                    return 0;
                }
                

                if (sign == '(')
                {
                    tk->type = token_left_bracket;
                    
                    return 0;
                }


                if (sign == ')')
                {
                    tk->type = token_right_bracket;
                    
                    return 0;
                }


                if (sign == ',')
                {
                    tk->type = token_comma;
                    
                    return 0;
                }


                if (sign == ':')
                {
                    tk->type = token_colon;
                    
                    return 0;
                }
            

                if (sign == ';')
                {
                    tk->type = token_semicolon;

                    return 0;
                }


                if (sign == '{')
                {
                    tk->type = token_left_curly_bracket;

                    return 0;
                }


                if (sign == '}')
                {
                    tk->type = token_right_curly_bracket;

                    return 0;
                }


                if (sign == '.')
                {
                    tk->type = token_dot;

                    return 0;
                }


                if (sign == '?')
                {
                    int next = getchar_modified();
                    if (next == 'f' || next == 'i' || next == 's')
                        tk->questionmark = true;
                        
                    ungetc(next, s_Fptr);

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
                    tk->type = token_EOF;
                    
                    return 0;
                }

                PRINT_ERROR_RET(ERROR_LEXICAL, "%d:%d: invalid sign: \"%c\"", line_counter, sign_counter, sign);
                
                
            /* Handling a situation with the prologue. */
            case(STATE_PROLOG_ONE): 
                if (sign == 'p')
                    current_state = STATE_PROLOG_TWO;
                else
                {
                    str_add_sign(&tk->string, sign);
                    PRINT_ERROR_RET(ERROR_LEXICAL, "wrong prolog");
                }
                break;
            
            case(STATE_PROLOG_TWO):
                if (sign == 'h')
                    current_state = STATE_PROLOG_THREE;
                else
                {
                    str_add_sign(&tk->string, sign);
                    PRINT_ERROR_RET(ERROR_LEXICAL, "wrong prolog");
                }
                break;

            case(STATE_PROLOG_THREE):
                if (sign == 'p')
                    current_state = STATE_PROLOG;
                else
                {
                    str_add_sign(&tk->string, sign);
                    PRINT_ERROR_RET(ERROR_LEXICAL, "wrong prolog");
                }
                break;

            case(STATE_PROLOG):
                tk->type = token_prologue;
                ungetc(sign, s_Fptr);
                return 0;

            /* Handling a situation with a ID type or epilogue. */
            case(STATE_QUESTION_MARK):
                if (sign == '>')
                    current_state = STATE_END;

                else if (sign == 'f' || sign == 'i' || sign == 's')
                {
                    current_state = STATE_ID_OR_KEYWORD;
                    str_add_sign(&tk->string, sign);
                    break;
                }
                else
                {
                    str_add_sign(&tk->string, sign);
                    PRINT_ERROR_RET(ERROR_LEXICAL, "wrong character after '?'");   
                }

                break;

            /* Epilogue. */
            case(STATE_END):
                if (sign == EOF)
                {
                    tk->type = token_end;
                    ungetc(sign, s_Fptr);
                }
                else
                {
                    str_add_sign(&tk->string, sign);
                    PRINT_ERROR_RET(ERROR_LEXICAL, "Character after the closing squence '?>'");
                }
                return 0;


            /* Handles a word  */
            case(STATE_ID_OR_KEYWORD):
                if (str_last_sign(&tk->string) == '$' && isdigit(sign))
                    PRINT_ERROR_RET(ERROR_LEXICAL, "Wrong ID or keyword. (first letter can't be digit)");
                while(true)
                {
                    if (isalpha(sign) || isdigit(sign) || sign == '_')
                        str_add_sign(&tk->string, sign);
                    else
                        break;
                    
                    sign = getchar_modified();
                }
                determine_type(tk);
                ungetc(sign, s_Fptr); //Returns the sign back to stream. 

                return 0;


            /* Handles a number and sorts by integer, decimal, or exponent. */
            case(STATE_NUMBER):
                while(true)
                {
                    if (isdigit(sign))
                        str_add_sign(&tk->string, sign);
                    else if ( sign == '.')
                    {
                        str_add_sign(&tk->string, sign);
                        is_sign = true;
                        current_state = STATE_NUMBER_POINT;
                        break;
                    }
                    else if (sign == 'E' || sign == 'e')
                    {
                        str_add_sign(&tk->string, sign);
                        current_state = STATE_NUMBER_EXPONENT_START;
                        is_sign = true;
                        break;
                    }
                    else if (isspace(sign) || sign == '\n' || sign == EOF || sign == '\t')
                        break;
                    else if (sign == '-' || sign == '+' || sign == '*' || sign == '/' || sign == '<' || sign == '>' || sign == '=' ||
                             sign == '~' || sign == '(' || sign == ')' || sign == ',' || sign == ';' || sign == '!' ) 
                        break;
                    else if (!isdigit(sign))
                    { 
                        str_add_sign(&tk->string, sign);
                        PRINT_ERROR_RET(ERROR_LEXICAL, "%d:%d: wrong form of integer: \"%s\"", line_counter, sign_counter, tk->string.ptr);
                    }
                    
                    sign = getchar_modified();
                }    

                if (is_sign == true)
                    break;

                tk->integer = atoi(tk->string.ptr); // Converts string to int.
                tk->type = token_integer;
                current_state = STATE_START;
                ungetc(sign, s_Fptr);
                
                return 0;


            /* Handles an exponent. */
            case(STATE_NUMBER_EXPONENT_START):
                if ( sign == '+' || sign == '-' )
                {
                    str_add_sign(&tk->string, sign);
                    current_state = STATE_NUMBER_EXPONENT_SIGN;
                    break;
                } 
                else if ( isdigit(sign) )
                {
                    str_add_sign(&tk->string, sign);
                    current_state = STATE_NUMBER_EXPONENT;
                    break;
                }
                else 
                {
                    str_add_sign(&tk->string, sign);
                    PRINT_ERROR_RET(ERROR_LEXICAL, "%d:%d: wrong form of exponent: \"%s\"", line_counter, sign_counter, tk->string.ptr);
                }


            /* Handles an exponent with plus or minus signs. */
            case(STATE_NUMBER_EXPONENT_SIGN):
                if ( isdigit(sign) )
                {
                    str_add_sign(&tk->string, sign);
                    current_state = STATE_NUMBER_EXPONENT;
                    break;
                } 
                else
                {
                    str_add_sign(&tk->string, sign);
                    PRINT_ERROR_RET(ERROR_LEXICAL, "%d:%d: wrong form of exponent: \"%s\"", line_counter, sign_counter, tk->string.ptr);
                }
            

            /* The last stage of processing the exponent. */
            case(STATE_NUMBER_EXPONENT):
                while(true)
                {
                    if (isdigit(sign))
                        str_add_sign(&tk->string, sign);
                    else if (isspace(sign) || sign == EOF || sign == ';' || sign == ')' || sign == ',') 
                        break; 
                    else
                    {
                        str_add_sign(&tk->string, sign);
                        PRINT_ERROR_RET(ERROR_LEXICAL, "%d:%d: wrong form of exponent: \"%s\"", line_counter, sign_counter, tk->string.ptr);
                    }

                    sign = getchar_modified();                      
                }
                tk->type = token_float;
                tk->decimal = atof(tk->string.ptr); // Converts string to double.
                ungetc(sign, s_Fptr);
                
                return 0;


            /* Handles a floating point number. */
            case(STATE_NUMBER_POINT):

                if (isdigit(sign)) 
                {
                    str_add_sign(&tk->string, sign);
                    current_state = STATE_NUMBER_DOUBLE;
                }
                else
                {
                    str_add_sign(&tk->string, sign);
                    PRINT_ERROR_RET(ERROR_LEXICAL, "%d:%d: wrong form of decimal: \"%s\"", line_counter, sign_counter, tk->string.ptr);   
                }
                break;


            /* Continues to handle a floating point number. */
            case(STATE_NUMBER_DOUBLE):
                while(true)
                {
                    if (isdigit(sign))
                        str_add_sign(&tk->string, sign);
                    else if (sign == 'E' || sign == 'e')
                    {
                        str_add_sign(&tk->string, sign);
                        current_state = STATE_NUMBER_EXPONENT_START;
                        is_exponent = true;
                        break;
                    }
                    else if (isspace(sign) || sign == '\n' || sign == EOF || sign == '\t')
                        break;
                    else if (sign == '-' || sign == '+' || sign == '*' || sign == '/' || sign == '<' || sign == '>' || 
                            sign == '=' || sign == '~' || sign == '(' || sign == ')' || sign == ',' || sign == ';' )
                        break;
                    else if (!isdigit(sign))
                    {
                        str_add_sign(&tk->string, sign);
                        PRINT_ERROR_RET(ERROR_LEXICAL, "%d:%d: wrong form of decimal: \"%s\"", line_counter, sign_counter, tk->string.ptr);
                    }
                    
                    sign = getchar_modified();
                }  

                if (is_exponent == true) break;
                tk->decimal = atof(tk->string.ptr); // Converts string to double.
                tk->type = token_float;
                current_state = STATE_START;
                ungetc(sign, s_Fptr);
                
                return 0;

            
            /* Handling the situation with relational operator '<'. */
            case(STATE_LESS):
                if (sign == '=')
                    current_state = STATE_LESS_OR_EQUAL;
                else if (sign == '?')
                {
                    current_state = STATE_PROLOG_ONE;
                }
                else
                {
                    ungetc(sign, s_Fptr);
                    tk->type = token_less;
                    
                    return 0;
                }
                break;
            

            /* Handling the situation with relational operator '<='. */
            case(STATE_LESS_OR_EQUAL):
                tk->type = token_less_or_equal;
                ungetc(sign, s_Fptr);
                
                return 0;

            
            /* Handling the situation with relational operator '>'. */
            case(STATE_GREATER):
                if (sign == '=')
                    current_state = STATE_GREATER_OR_EQUAL;
                else
                    {
                        ungetc(sign, s_Fptr);
                        tk->type = token_greater;
                        
                        return 0;
                    }
                break;


            /* Handling the situation with relational operator '>='. */
            case(STATE_GREATER_OR_EQUAL):
                tk->type = token_greater_or_equal;
                ungetc(sign, s_Fptr);
                
                return 0;


            /* Handling the situation with the equals sign '='. */
            case(STATE_EQUAL_SIGN):
                if (sign == '=')
                    current_state = STATE_IS_EQUAL;
                else
                {
                    ungetc(sign, s_Fptr);
                    tk->type = token_equal_sign; 
                    
                    return 0;
                }
                break;


            /* Handling the situation with '=='. */
            case(STATE_IS_EQUAL):
                if(sign == '=')
                    current_state = STATE_IS_EQUAL_END;
                else
                {
                    str_add_sign(&tk->string, sign);
                    PRINT_ERROR_RET(ERROR_LEXICAL, "%d:%d: wrong сharacter \"%s\"", line_counter, sign_counter, tk->string.ptr);    
                }
                
                break;


            /* Handling the situation with relational operator '==='. */
            case(STATE_IS_EQUAL_END):
                tk->type = token_equal; 
                ungetc(sign, s_Fptr);

                return 0;

            
            /* Handling the situation with '!'. */
            case(STATE_NOT_EQUAL_START):
                if (sign == '=')
                    current_state = STATE_NOT_EQUAL;
                else
                {
                    str_add_sign(&tk->string,'!');
                    str_add_sign(&tk->string,sign);
                    PRINT_ERROR_RET(ERROR_LEXICAL, "%d:%d: unsuitable combination of characters: \"%s\"", line_counter, sign_counter, tk->string.ptr);
                }
                break;
            

            /* Handling the situation with '!='. */
            case(STATE_NOT_EQUAL):
                if(sign == '=')
                     current_state = STATE_NOT_EQUAL_END;
                else
                {
                    str_add_sign(&tk->string,'!');
                    str_add_sign(&tk->string,sign);
                    PRINT_ERROR_RET(ERROR_LEXICAL, "%d:%d: unsuitable combination of characters: \"%s\"", line_counter, sign_counter, tk->string.ptr);
                }
                break;


            /* Handling the situation with relational operator '!=='. */
            case(STATE_NOT_EQUAL_END):    
                tk->type = token_not_equal;
                ungetc(sign, s_Fptr);
                
                return 0;


            /* Handling the situation with division sign '/'. */
            case(STATE_DIVISION):
                if (sign == '/') // so this is a single line comment.
                    current_state = STATE_COMMENT;
                else if (sign == '*') // so this is a block comment.
                    current_state = STATE_COMMENT_BLOCK_START;
                else
                {
                    ungetc(sign, s_Fptr);
                    tk->type = token_divide;
                    
                    return 0; 
                }
                break;


            /* Handling the situation with line comment '//'. */
            case(STATE_COMMENT):
                while(true)
                {
                    if (sign == EOF || sign == '\n')
                    {
                        ungetc(sign, s_Fptr);
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


            /* Handling the situation with the start of a block comment '/*'. */
            case(STATE_COMMENT_BLOCK_START): 
                while(true)
                {
                    if (sign == EOF)
                        return ERROR_LEXICAL;
                    else if (sign != '*') 
                        sign = getchar_modified();
                    else if (sign == '*')
                    {
                        ungetc(sign, s_Fptr);
                        current_state = STATE_COMMENT_BLOCK_END;
                        break;
                    }
                }
                break;


            /* Handling the situation with a comment at the end of a block '/**\/' */
            case(STATE_COMMENT_BLOCK_END):
                while(true)
                {
                    if (sign == '/')
                    {
                        current_state = STATE_START;
                        break;
                    }
                    else if (sign == EOF)
                        return ERROR_LEXICAL;
                    else if (sign == '*')
                        sign = getchar_modified();
                    else
                    {
                        current_state = STATE_COMMENT_BLOCK_START;
                        ungetc(sign, s_Fptr);
                        break;
                    }
                    
                }
                break;


            /* Handling the situation with the beginning of a line '"' */
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
                    else if (sign == '$')
                    {
                        PRINT_ERROR_RET(ERROR_LEXICAL, "%d:%d: unterminated string", line_counter, sign_counter);
                        
                    }
                    else if (sign < 32)
                        PRINT_ERROR_RET(ERROR_LEXICAL, "%d:%d: string error", line_counter, sign_counter);
                        
                    str_add_sign(&tk->string, sign);
                    sign = getchar_modified();
                }
                break;
            

            /* Handling the situation with an escape sequence in a string '\'. */
            case(STATE_STRING_BACKSLASH):
                if (sign == '\"')
                {
                    current_state = STATE_STRING_START;
                    str_add_sign(&tk->string, sign);
                }
                else if (sign == 'n')
                {
                    current_state = STATE_STRING_START;
                    str_add_sign(&tk->string, '\n'); 
                }
                else if (sign == 't')
                {
                    current_state = STATE_STRING_START;
                    str_add_sign(&tk->string, '\t');
                }
                else if (sign == '\\')
                {
                    current_state = STATE_STRING_START;
                    str_add_sign(&tk->string, '\\');
                }
                else if (sign == '$') 
                {
                    current_state = STATE_STRING_START;
                    str_add_sign(&tk->string, '$');    
                }
                else if (sign == '0')
                {
                    current_state = STATE_STRING_BACKSLASH_ZERO;
                    ascii_oct[0] = sign; 
                }
                else if (sign == '1' || sign == '2' || sign == '3')
                {
                    current_state = STATE_STRING_BACKSLASH_ONE_TWO_THREE;
                    ascii_oct[0] = sign; 
                }
                else if (sign == 'x')
                {
                    current_state = STATE_STRING_BACKSLASH_HEX;
                }    
                else if (sign < 32)
                    PRINT_ERROR_RET(ERROR_LEXICAL, "%d:%d: string error", line_counter, sign_counter);
                else
                {
                    current_state = STATE_STRING_START; 
                    str_add_sign(&tk->string, '\\');
                    str_add_sign(&tk->string, sign);
                }
                break;


            /* Handling the situation with an escape sequence \[oct][][]. */
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
                else if (sign < 32)
                    PRINT_ERROR_RET(ERROR_LEXICAL, "%d:%d: string error", line_counter, sign_counter);
                else
                {
                    str_add_sign(&tk->string, '\\');
                    str_add_sign(&tk->string, ascii_oct[0]);
                    ungetc(sign, s_Fptr);
                    current_state = STATE_STRING_START;
                    break;
                }


            /* Handling the situation with an escape sequence \[oct][oct][]. */
            case(STATE_STRING_BACKSLASH_ZERO_ZERO):
                if (sign >= '1' && sign <= '7')
                {
                    current_state = STATE_STRING_START;
                    ascii_oct[2] = sign;
                    str_add_sign(&tk->string, strtol(ascii_oct, NULL, 8));
                    break;
                }
                else if (sign < 32)
                    PRINT_ERROR_RET(ERROR_LEXICAL, "%d:%d: string error", line_counter, sign_counter);
                else
                {
                    str_add_sign(&tk->string, '\\');
                    str_add_sign(&tk->string, ascii_oct[0]);
                    str_add_sign(&tk->string, ascii_oct[1]);
                    ungetc(sign, s_Fptr);
                    current_state = STATE_STRING_START;
                    break;
                }


            /* Handling the situation with an escape sequence \[oct][][]. */
            case(STATE_STRING_BACKSLASH_ONE_TWO_THREE):
                if (sign >= '0' && sign <= '7')
                {
                    current_state = STATE_STRING_BACKSLASH_ZERO_TO_SEVEN;
                    ascii_oct[1] = sign;
                    break;
                }
                else if (sign < 32)
                    PRINT_ERROR_RET(ERROR_LEXICAL, "%d:%d: string error", line_counter, sign_counter);
                else 
                {
                    str_add_sign(&tk->string, '\\');
                    str_add_sign(&tk->string, ascii_oct[0]);
                    ungetc(sign, s_Fptr);
                    current_state = STATE_STRING_START;
                    break;
                }
                    

            /* Handling the situation with an escape sequence \[oct][oct][]. */
            case(STATE_STRING_BACKSLASH_ZERO_TO_SEVEN):
                if (sign >= '0' && sign <= '7')
                {   
                    current_state = STATE_STRING_START;
                    ascii_oct[2] = sign;
                    str_add_sign(&tk->string, strtol(ascii_oct, NULL, 8));
                    break;
                }
                else if (sign < 32)
                    PRINT_ERROR_RET(ERROR_LEXICAL, "%d:%d: string error", line_counter, sign_counter);
                else 
                {
                    str_add_sign(&tk->string, '\\');
                    str_add_sign(&tk->string, ascii_oct[0]);
                    str_add_sign(&tk->string, ascii_oct[1]);
                    ungetc(sign, s_Fptr);
                    current_state = STATE_STRING_START;
                    break;
                }


            /* Handling the situation with an escape sequence \x[hex][]. */
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
                else if (sign < 32)
                    PRINT_ERROR_RET(ERROR_LEXICAL, "%d:%d: string error", line_counter, sign_counter);
                else 
                {
                    str_add_sign(&tk->string, '\\');
                    str_add_sign(&tk->string, 'x');
                    ungetc(sign, s_Fptr);
                    current_state = STATE_STRING_START;
                    break;
                }


            /* Handling the situation with an escape sequence \x[hex][hex]. */
            case(STATE_STRING_BACKSLASH_HEX_FIRST):
                if (sign >= '0' && sign <= '9' || sign >= 'a' && sign <= 'f' || sign >= 'A' && sign <= 'F')
                {
                    current_state = STATE_STRING_START;
                    ascii_hex[1] = sign;
                    str_add_sign(&tk->string, strtol(ascii_hex, NULL, 16));

                    break;
                }
                else if (sign < 32)
                    PRINT_ERROR_RET(ERROR_LEXICAL, "%d:%d: string error", line_counter, sign_counter);
                else 
                {
                    str_add_sign(&tk->string, '\\');
                    str_add_sign(&tk->string, 'x');
                    str_add_sign(&tk->string, ascii_hex[0]);
                    ungetc(sign, s_Fptr);
                    current_state = STATE_STRING_START;
                    break;
                }


            /* Handling the situation with an escape sequence \x[hex][hex]. */
            case(STATE_STRING_BACKSLASH_HEX_ZERO):
                if (sign >= '1' && sign <= '9' || sign >= 'a' && sign <= 'f' || sign >= 'A' && sign <= 'F')
                {
                    current_state = STATE_STRING_START;
                    ascii_hex[1] = sign;
                    str_add_sign(&tk->string, strtol(ascii_hex, NULL, 16));

                    break;
                }
                else if (sign < 32)
                    PRINT_ERROR_RET(ERROR_LEXICAL, "%d:%d: string error", line_counter, sign_counter);
                else 
                {
                    str_add_sign(&tk->string, '\\');
                    str_add_sign(&tk->string, 'x');
                    str_add_sign(&tk->string, ascii_hex[0]);
                    ungetc(sign, s_Fptr);
                    current_state = STATE_STRING_START;
                    break;
                }


            /* Handling the situation with end of string '"'. */
            case(STATE_STRING):
                tk->type = token_string;
                current_state = STATE_START;
                ungetc(sign, s_Fptr);
                
                return 0;
        }
    }
}