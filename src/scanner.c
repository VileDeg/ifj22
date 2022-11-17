#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "scanner.h"
#include "errors.h"


// File to read from
static FILE* s_Fptr;

// Creates dynamic string.
static str_t* s_String;

static unsigned int sign_counter = 0;
static unsigned int line_counter = 1;
bool is_correct_file_start = false;

void scanner_set_file(FILE* fptr)
{
    s_Fptr = fptr;
}

FILE* scanner_get_file()
{
    return s_Fptr;
}

void scanner_set_string(str_t* str)
{
    s_String = str;
}

// void scanner_reset()
// {
//     sign_counter = 0;
//     line_counter = 1;
// }

//Comparing string we've gotten and compares with KW. In case it isn't a KW -> it's an ID.
bool determine_type(Token* tk)
{
    //Whether an identifier has been set.
    bool flag_id = false;
    bool flag_null = false;
    str_t* str = tk->string;
    // if (str[0] == '?')
    // {
    //     tk->type = token_keyword;
    //     return 0;
    // }

    if (str_cmp(str, "else"))
        tk->keyword = keyword_else;
    else if (str_cmp(str, "float") || str_cmp(str, "?float"))
        tk->keyword = keyword_float;  
    else if (str_cmp(str, "int") || str_cmp(str, "?int"))
        tk->keyword = keyword_int;
    else if (str_cmp(str, "string") || str_cmp(str, "?string"))
        tk->keyword = keyword_string;
    else if (str_cmp(str, "function"))
        tk->keyword = keyword_function;
    else if (str_cmp(str, "if"))
        tk->keyword = keyword_if;
    else if (str_cmp(str, "return"))
        tk->keyword = keyword_return;
    else if (str_cmp(str, "void"))
        tk->keyword = keyword_void;
    else if (str_cmp(str, "while"))
        tk->keyword = keyword_while;
    else if (str_cmp(str, "null"))
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

    return 0; //Scanner token was successful.
}

//Gets a character from stdin and track location of lines and signs.
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

// void token_clear(Token *tk)
// {
//     memset(tk, 0, sizeof(Token));
// }

//Function for reading string from stdin and converting into token.
int scanner_get_next_token(Token* token)
{
    str_clear(s_String);
    //token_clear(token);
    token->string = s_String;
    
    int current_state = STATE_START; 
    int sign;                 //Sign which is taken one by one from the input string.
    bool is_sign = false;     //For STATE_NUMBER case.
    bool is_exponent = false; //For STATE_NUMBER_DOUBLE case.
    char ascii_oct[3];        //For working with octal numbers in escape sequences.
    char ascii_hex[2];        //For working with hex numbers in escape sequences.
    
    while(true)
    {
        sign = getchar_modified();
        
        if(is_correct_file_start == false)
        {
            // if (sign != '<')
            // {
            //     PRINT_ERROR_RET(ERROR_LEXICAL, "wrong file start");
            //     //return ERROR_LEXICAL;
            // }
                
            is_correct_file_start = true;
        }

        switch(current_state)
        {
            //Handling a situation with a coincidence of signs.
            case(STATE_START):

                // if (sign == '\n')
                // {
                //     token->type = token_EOL;
                //     return 0;
                // }

                if (sign == '_' || isalpha(sign) || sign == '$')
                {
                    str_add_sign(s_String, sign);
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
                    str_add_sign(s_String, sign);
                    current_state = STATE_NUMBER;
                    break;
                }


                if (sign == '+')
                {
                    token->type = token_plus;
                    
                    return 0;
                }

                if (sign == '-')
                {
                    token->type = token_minus;
                    
                    return 0;
                }


                if (sign == '*')
                {
                    token->type = token_multiply;
                    
                    return 0;
                }
                

                if (sign == '(')
                {
                    token->type = token_left_bracket;
                    
                    return 0;
                }


                if (sign == ')')
                {
                    token->type = token_right_bracket;
                    
                    return 0;
                }


                if (sign == ',')
                {
                    token->type = token_comma;
                    
                    return 0;
                }


                if (sign == ':')
                {
                    token->type = token_colon;
                    
                    return 0;
                }
            

                if (sign == ';')
                {
                    token->type = token_semicolon;

                    return 0;
                }
                    
                if (sign == '{')
                {
                    token->type = token_left_curly_bracket;

                    return 0;
                }

                if (sign == '}')
                {
                    token->type = token_right_curly_bracket;

                    return 0;
                }

                if (sign == '.')
                {
                    token->type = token_dot;

                    return 0;
                }
                
                if (sign == '?')
                {
                    int next = getchar_modified();
                    if (next == 'f' || next == 'i' || next == 's')
                        token->questionmark = true;
                        //str_add_sign(s_String, sign);
                        
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
                    token->type = token_EOF;
                    
                    return 0;
                }

                //If there were no signs matches, then a lexical error is written out.
                //PRINT_ERROR("[LEXICAL ERROR]:%d:%d: invalid sign: \"%c\"\n", line_counter, sign_counter, sign);
                PRINT_ERROR_RET(ERROR_LEXICAL, "%d:%d: invalid sign: \"%c\"", line_counter, sign_counter, sign);
                // fprintf(stderr, "\033[1;31m");  //!! Color of the error message.
                // fprintf(stderr, "[LEXICAL ERROR]:%d:%d: invalid sign: \"%c\"\n", line_counter, sign_counter, sign);
                // fprintf(stderr, "\033[0m");

                

            case(STATE_PROLOG_ONE):
                if (sign == 'p')
                {
                    current_state = STATE_PROLOG_TWO;
                }
                else
                {
                    //Error handling. 
                    str_add_sign(s_String, sign);
                    PRINT_ERROR_RET(ERROR_LEXICAL, "wrong prolog");
                    
                     
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
                    str_add_sign(s_String, sign);
                    PRINT_ERROR_RET(ERROR_LEXICAL, "wrong prolog");
                    
                     
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
                    str_add_sign(s_String, sign);
                    PRINT_ERROR_RET(ERROR_LEXICAL, "wrong prolog");
                    
                     
                }
                break;

            case(STATE_PROLOG):
                token->type = token_prologue;
                return 0;
                                    
            case(STATE_QUESTION_MARK):
                if (sign == '>')
                {
                    current_state = STATE_END;
                }
                else if (sign == 'f' || sign == 'i' || sign == 's')
                {
                    current_state = STATE_ID_OR_KEYWORD;
                    str_add_sign(s_String, sign);
                    break;
                }
                else
                {
                    //Error handling. 
                    str_add_sign(s_String, sign);
                    PRINT_ERROR_RET(ERROR_LEXICAL, "wrong character after '?'");
                    
                     
                }

                break;

            case(STATE_END):

                //проверка следующего символа(его не должно быть)
                if (sign == EOF)
                {
                    token->type = token_end;
                    ungetc(sign, s_Fptr);
                }
                else
                {
                    //Error handling. 
                    str_add_sign(s_String, sign);
                    PRINT_ERROR_RET(ERROR_LEXICAL, "Character after the closing squence '?>'");
                    
                     
                }
                
                return 0;


            
            //Handle a word and sorting it by ID or keyword.
            case(STATE_ID_OR_KEYWORD):
                if (str_last_sign(s_String) == '$' && isdigit(sign))
                    PRINT_ERROR_RET(ERROR_LEXICAL, "Wrong ID or keyword. (first letter can't be digit)");
                while(true)
                {
                    if (isalpha(sign) || isdigit(sign) || sign == '_')
                        str_add_sign(s_String, sign);
                    //  else if (isspace(sign) || sign == '\n' || sign == EOF || sign == '\t')
                    //     break;
                    // else if (sign == '-' ||  sign == '+' || sign == '*' || sign == '/' || sign == '<' || 
                    //          sign == '>' || sign == '=' || sign == '(' || sign == ')' ||  sign == ';' ) //sign == ',' || sign == '!'
                    //     break;
                    else
                    {
                        break;
                        PRINT_ERROR_RET(ERROR_LEXICAL, "Wrong ID or keyword.");
                         
                    }
                    sign = getchar_modified();
                }
                determine_type(token);
                //Returns the sign back to stream. 
                ungetc(sign, s_Fptr);

                return 0;

            //Handle a number and sorting it by integer, decimal or exponent.
            case(STATE_NUMBER):
                while(true)
                {
                    if (isdigit(sign))
                        str_add_sign(s_String, sign);
                    else if ( sign == '.')
                    {
                        str_add_sign(s_String, sign);
                        is_sign = true;
                        current_state = STATE_NUMBER_POINT;
                        break;
                    }
                    else if (sign == 'E' || sign == 'e')
                    {
                        str_add_sign(s_String, sign);
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
                        //Error handling. 
                        str_add_sign(s_String, sign);
                        PRINT_ERROR_RET(ERROR_LEXICAL, "%d:%d: wrong form of integer: \"%s\"", line_counter, sign_counter, token->string->ptr);
                        
                         
                    }
                    
                    sign = getchar_modified();
                }    

                if (is_sign == true)
                    break;

                //Converts string to int. 
                token->integer = atoi(s_String->ptr);
                token->type = token_integer;
                current_state = STATE_START;
                ungetc(sign, s_Fptr);
                
                return 0;

            //Handle an exponent.
            case(STATE_NUMBER_EXPONENT_START):
                if ( sign == '+' || sign == '-' )
                {
                    str_add_sign(s_String, sign);
                    current_state = STATE_NUMBER_EXPONENT_SIGN;
                    break;
                } 
                else if ( isdigit(sign) )
                {
                    str_add_sign(s_String, sign);
                    current_state = STATE_NUMBER_EXPONENT;
                    break;
                }
                // else if (isspace(sign) || sign == '\n' || sign == EOF || sign == '\t')
                // {
                //     current_state = STATE_NUMBER_EXPONENT;
                //     ungetc(sign, s_Fptr);
                //     break;
                // }
                else 
                {
                    str_add_sign(s_String, sign);
                    PRINT_ERROR_RET(ERROR_LEXICAL, "%d:%d: wrong form of exponent: \"%s\"", line_counter, sign_counter, token->string->ptr);
                    
                    
                }

            //Handle an exponent using plus or minus.
            case(STATE_NUMBER_EXPONENT_SIGN):
                if ( isdigit(sign) )
                {
                    str_add_sign(s_String, sign);
                    current_state = STATE_NUMBER_EXPONENT;
                    break;
                } 
                // else if (isspace(sign) || sign == '\n' || sign == EOF || sign == '\t')  //??
                // {
                //     current_state = STATE_NUMBER_EXPONENT;
                //     ungetc(sign, s_Fptr);
                //     break;
                // }
                else
                {
                    str_add_sign(s_String, sign);
                    PRINT_ERROR_RET(ERROR_LEXICAL, "%d:%d: wrong form of exponent: \"%s\"", line_counter, sign_counter, token->string->ptr);
                    
                    
                }
            
            //The last stage of processing the exponent.
            case(STATE_NUMBER_EXPONENT):
                while(true)
                {
                    if (isdigit(sign))
                        str_add_sign(s_String, sign);
                    else if (isspace(sign) || sign == '\n' || sign == EOF || sign == '\t' || sign == ';') // нужно больше исключений? 
                        break; 
                    else
                    {
                        str_add_sign(s_String, sign);
                        PRINT_ERROR_RET(ERROR_LEXICAL, "%d:%d: wrong form of exponent: \"%s\"", line_counter, sign_counter, token->string->ptr);
                        
                         
                    }
                         
                    sign = getchar_modified();                      
                }
                token->type = token_float;
                //Converts string to a floating-point number (decimal).
                token->decimal = atof(s_String->ptr);
                //!!printf("%f", atof("83e2"));
                ungetc(sign, s_Fptr);
                
                return 0;

            //First handle a floating-point number.
            case(STATE_NUMBER_POINT):

                if (isdigit(sign)) 
                {
                    str_add_sign(s_String, sign);
                    current_state = STATE_NUMBER_DOUBLE;
                }
                else
                {
                    str_add_sign(s_String, sign);
                    PRINT_ERROR_RET(ERROR_LEXICAL, "%d:%d: wrong form of decimal: \"%s\"", line_counter, sign_counter, token->string->ptr);
                    
                     
                }
                break;

            //Second handle a floating-point number.
            case(STATE_NUMBER_DOUBLE):
            while(true)
            {
                if (isdigit(sign))
                    str_add_sign(s_String, sign);
                else if (sign == 'E' || sign == 'e')
                {
                    str_add_sign(s_String, sign);
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
                    str_add_sign(s_String, sign);
                    PRINT_ERROR_RET(ERROR_LEXICAL, "%d:%d: wrong form of decimal: \"%s\"", line_counter, sign_counter, token->string->ptr);
                    
                     
                }
                
                sign = getchar_modified();
            }        
            if (is_exponent == true) break;
            token->decimal = atof(s_String->ptr);
            token->type = token_float;

            current_state = STATE_START;
            ungetc(sign, s_Fptr);
            
            return 0;
/*    
            //Handling a situation with division sign '/'.
            case(STATE_DIVISION):
                if (sign == '/')
                    current_state = STATE_DIVISION_INTEGER;
                else
                    {
                        ungetc(sign, s_Fptr);
                        token->type = token_divide;
                        
                        return 0;
                    }
                break;

            //Handling a situation with division integer "//".
            case(STATE_DIVISION_INTEGER):
                token->type = token_divide_integer;
                ungetc(sign, s_Fptr);
                
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
                    ungetc(sign, s_Fptr);
                    token->type = token_less;
                    
                    return 0;
                }
                break;
            
            //Handling a situation with relation operator "<=".
            case(STATE_LESS_OR_EQUAL):
                token->type = token_less_or_equal;
                ungetc(sign, s_Fptr);
                
                return 0;

            
            //Handling a situation with relation operator '>'.
            case(STATE_GREATER):
                if (sign == '=')
                    current_state = STATE_GREATER_OR_EQUAL;
                else
                    {
                        ungetc(sign, s_Fptr);
                        token->type = token_greater;
                        
                        return 0;
                    }
                break;

            //Handling a situation with relation operator ">=".
            case(STATE_GREATER_OR_EQUAL):
                token->type = token_greater_or_equal;
                ungetc(sign, s_Fptr);
                
                return 0;


            //Handling a situation with equal sign '='.
            case(STATE_EQUAL_SIGN):
                if (sign == '=')
                    current_state = STATE_IS_EQUAL;
                else
                {
                    ungetc(sign, s_Fptr);
                    token->type = token_equal_sign; 
                    
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
                    str_add_sign(s_String, sign);
                    PRINT_ERROR_RET(ERROR_LEXICAL, "%d:%d: wrong сharacter \"%s\"", line_counter, sign_counter, token->string->ptr);
                    
                     
                }
                
                break;

            //Handling a situation with relation operator '==='.
            case(STATE_IS_EQUAL_END):
                token->type = token_equal; 
                ungetc(sign, s_Fptr);

                return 0;

            
            //Handling a situation with start of relation operator "!=".
            case(STATE_NOT_EQUAL_START):
                if (sign == '=')
                    current_state = STATE_NOT_EQUAL;
                else
                {
                    str_add_sign(s_String,'!');
                    str_add_sign(s_String,sign);
                    PRINT_ERROR_RET(ERROR_LEXICAL, "%d:%d: unsuitable combination of characters: \"%s\"", line_counter, sign_counter, token->string->ptr);
                    
                    
                }
                break;
            
            //Handling a situation with relation operator "!=".
            case(STATE_NOT_EQUAL):
                if(sign == '=')
                     current_state = STATE_NOT_EQUAL_END;
                else
                {
                    str_add_sign(s_String,'!');
                    str_add_sign(s_String,sign);
                    PRINT_ERROR_RET(ERROR_LEXICAL, "%d:%d: unsuitable combination of characters: \"%s\"", line_counter, sign_counter, token->string->ptr);
                    
                    
                }
                break;

            //Handling a situation with relation operator "!==".
            case(STATE_NOT_EQUAL_END):    
                token->type = token_not_equal;
                ungetc(sign, s_Fptr);
                
                return 0;

            //Handling a situation with minus sign '/'.    
            case(STATE_DIVISION):
                if (sign == '/') //значит это однострочный комментарий 
                    current_state = STATE_COMMENT;
                else if (sign == '*') //значит это многострочный комментарий
                    current_state = STATE_COMMENT_BLOCK_START;
                else
                {
                    ungetc(sign, s_Fptr);
                    token->type = token_divide;
                    
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

            //Handling a situation with start of block comment "/*".
            case(STATE_COMMENT_BLOCK_START): 
                while(true)
                {
                    if (sign == EOF)
                    {
                        return ERROR_LEXICAL;
                        // current_state = STATE_START;
                        // ungetc(sign, s_Fptr);
                        // break;
                    }
                    else if (sign != '*') 
                        sign = getchar_modified();
                    else if (sign == '*')
                    {
                        sign = getchar_modified();
                        if (sign == '/')
                        {
                            current_state = STATE_START;
                            break;
                        }
                    }
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
                    ungetc(sign, s_Fptr);
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
                    else if (sign == '$')
                    {
                        PRINT_ERROR_RET(ERROR_LEXICAL, "%d:%d: unterminated string", line_counter, sign_counter);
                        
                    }
                    // else if (sign == '\n') //!! надо чтоб считывало \n знак в строке? 
                    //     break;
                    else if (sign == EOF) //(sign == '\n' || sign == EOF)
                    {
                        PRINT_ERROR_RET(ERROR_LEXICAL, "%d:%d: no end of string", line_counter, sign_counter);
                        
                        
                    }
                    
                    str_add_sign(s_String, sign);
                    sign = getchar_modified();
                }
                break;
            
            //Handling a situation with escape sequence in string '\'.
            case(STATE_STRING_BACKSLASH):
                if (sign == '\"')
                {
                    current_state = STATE_STRING_START;
                    str_add_sign(s_String, sign);
                }
                else if (sign == 'n')
                {
                    current_state = STATE_STRING_START;
                    str_add_sign(s_String, '\n'); 
                }
                else if (sign == 't')
                {
                    current_state = STATE_STRING_START;
                    str_add_sign(s_String, '\t');
                }
                else if (sign == '\\')
                {
                    current_state = STATE_STRING_START;
                    str_add_sign(s_String, '\\');
                }
                // else if (sign == '$') //???
                // {
                //     current_state = STATE_STRING_START;
                //     str_add_sign(s_String, '$');    
                // }
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
                else 
                {
                    current_state = STATE_STRING_START; 
                    str_add_sign(s_String, '\\');
                    str_add_sign(s_String, sign);
                    //PRINT_ERROR_LEX("%d:%d: invalid escape sequence: \"%s\"", line_counter, sign_counter, token->String->ptr);
                    
                    //return ERROR_LEXICAL;
                }
                break;


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
                    str_add_sign(s_String, '\\');
                    str_add_sign(s_String, ascii_oct[0]);
                    ungetc(sign, s_Fptr);
                    current_state = STATE_STRING_START;
                    break;

                    // PRINT_ERROR_LEX("%d:%d: invalid escape sequence", line_counter, sign_counter);

                    // return ERROR_LEXICAL;
                }

            case(STATE_STRING_BACKSLASH_ZERO_ZERO):
                if (sign >= '1' && sign <= '7')
                {
                    //[d][d][d].
                    current_state = STATE_STRING_START;
                    ascii_oct[2] = sign;
                    //str_add_sign(s_String, atoi(ascii_oct));
                    str_add_sign(s_String, strtol(ascii_oct, NULL, 8));
                    break;
                }
                else
                {
                    str_add_sign(s_String, '\\');
                    str_add_sign(s_String, ascii_oct[0]);
                    str_add_sign(s_String, ascii_oct[1]);
                    ungetc(sign, s_Fptr);
                    current_state = STATE_STRING_START;
                    break;
                    // PRINT_ERROR_LEX("%d:%d: invalid escape sequence", line_counter, sign_counter);

                    // return ERROR_LEXICAL;
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
                    str_add_sign(s_String, '\\');
                    str_add_sign(s_String, ascii_oct[0]);
                    ungetc(sign, s_Fptr);
                    current_state = STATE_STRING_START;
                    break;
                    // PRINT_ERROR_LEX("%d:%d: invalid escape sequence", line_counter, sign_counter);
                    
                    // return ERROR_LEXICAL;
                }
                    

            //Handling a situation with escape sequence ascii[d][d][].
            case(STATE_STRING_BACKSLASH_ZERO_TO_SEVEN):
                if (sign >= '0' && sign <= '7')
                {   
                    //[d][d][d].
                    current_state = STATE_STRING_START;
                    ascii_oct[2] = sign;
                    //str_add_sign(s_String, atoi(ascii_oct));
                    str_add_sign(s_String, strtol(ascii_oct, NULL, 8));
                    break;
                }
                else 
                {
                    str_add_sign(s_String, '\\');
                    str_add_sign(s_String, ascii_oct[0]);
                    str_add_sign(s_String, ascii_oct[1]);
                    ungetc(sign, s_Fptr);
                    current_state = STATE_STRING_START;
                    break;
                    // PRINT_ERROR_LEX("%d:%d: invalid escape sequence", line_counter, sign_counter);
                    
                    // return ERROR_LEXICAL;
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
                    str_add_sign(s_String, '\\');
                    str_add_sign(s_String, 'x');
                    ungetc(sign, s_Fptr);
                    current_state = STATE_STRING_START;
                    break;
                    // PRINT_ERROR_LEX("%d:%d: invalid escape sequence", line_counter, sign_counter);
                    
                    // return ERROR_LEXICAL;
                }

            //Handling a situation with escape sequence ascii[d][d].
            case(STATE_STRING_BACKSLASH_HEX_FIRST):
                if (sign >= '0' && sign <= '9' || sign >= 'a' && sign <= 'f' || sign >= 'A' && sign <= 'F')
                {
                    current_state = STATE_STRING_START;
                    ascii_hex[1] = sign;
                    str_add_sign(s_String, strtol(ascii_hex, NULL, 16));

                    break;
                }
                else 
                {
                    str_add_sign(s_String, '\\');
                    str_add_sign(s_String, 'x');
                    str_add_sign(s_String, ascii_hex[0]);
                    ungetc(sign, s_Fptr);
                    current_state = STATE_STRING_START;
                    break;
                    // PRINT_ERROR_LEX("%d:%d: invalid escape sequence", line_counter, sign_counter);
                    
                    // return ERROR_LEXICAL;
                }

            case(STATE_STRING_BACKSLASH_HEX_ZERO):
                if (sign >= '1' && sign <= '9' || sign >= 'a' && sign <= 'f' || sign >= 'A' && sign <= 'F')
                {
                    current_state = STATE_STRING_START;
                    ascii_hex[1] = sign;
                    str_add_sign(s_String, strtol(ascii_hex, NULL, 16));

                    break;
                }
                else 
                {
                    str_add_sign(s_String, '\\');
                    str_add_sign(s_String, 'x');
                    str_add_sign(s_String, ascii_hex[0]);
                    ungetc(sign, s_Fptr);
                    current_state = STATE_STRING_START;
                    break;
                    // PRINT_ERROR_LEX("%d:%d: invalid escape sequence", line_counter, sign_counter);
                    
                    // return ERROR_LEXICAL;
                }


            //Handling a situation with end of string '"'.
            case(STATE_STRING):
                token->type = token_string;
                current_state = STATE_START;
                ungetc(sign, s_Fptr);
                
                return 0;

        }
    }
}