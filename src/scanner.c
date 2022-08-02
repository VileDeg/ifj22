#include "base.h"

#include "scanner.h"
#include "scanner_dynamic_string.h"
//#include "errors.h"          

#define ERROR_INTERNAL -11
#define ERROR_LEXICAL  -111

// File to read from
static FILE* s_fptr;

// Creates dynamic string.
static str_t *String;

static unsigned int sign_counter = 0;
static unsigned int line_counter = 1;

void scanner_set_file(FILE* fptr)
{
    if (!fptr)
    {
        ERRPR("File pointer is NULL.");
        return;
    }
    s_fptr = fptr;
}

void scanner_init()
{
    s_fptr = stdin;
    String = s_calloc(sizeof(*String));
    //CALLOC(String, sizeof(*String));
    str_const(String);
    // if (str_init(String) == false)
    // {
    //     MEMFAIL;
    // }
}

void scanner_reset()
{
    sign_counter = 0;
    line_counter = 1;
}

void scanner_terminate()
{
    str_dest(String);
    S_FREE(String);
}

//Comparing string we've gotten and compares with KW. In case it isn't a KW -> it's an ID.
bool determine_type(str_t *String, Token *Token)
{
    //Whether an identifier has been set.
    bool flag_id = false;

    if (str_cmp(String, "do"))
        Token->keyword = keyword_do;
    else if (str_cmp(String, "else"))
        Token->keyword = keyword_else;  
    else if (str_cmp(String, "end"))
        Token->keyword = keyword_end;
    else if (str_cmp(String, "function"))
        Token->keyword = keyword_function;
    else if (str_cmp(String, "global"))
        Token->keyword = keyword_global;
    else if (str_cmp(String, "if"))
        Token->keyword = keyword_if;
    else if (str_cmp(String, "integer"))
        Token->keyword = keyword_integer;
    else if (str_cmp(String, "local"))
        Token->keyword = keyword_local;
    else if (str_cmp(String, "nil"))
        Token->keyword = keyword_nil;
    else if (str_cmp(String, "number"))
        Token->keyword = keyword_number;
    else if (str_cmp(String, "require"))
        Token->keyword = keyword_require;
    else if (str_cmp(String, "return"))
        Token->keyword = keyword_return;
    else if (str_cmp(String, "string"))
        Token->keyword = keyword_string;
    else if (str_cmp(String, "then"))
        Token->keyword = keyword_then;
    else if (str_cmp(String, "while"))
        Token->keyword = keyword_while;
    else
    {
        Token->type_of_token = token_ID;
        flag_id = true;
    } 

    if (flag_id == false)
        Token->type_of_token = token_keyword;
    
    //string_free(String);

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
int next_token(Token *Token)
{
    //Dynamic string declaration.  
    //str_t *String = calloc(1, sizeof(struct dyn_str));
    
    // if(String == NULL)
    // {
    //     //Malloc check 
    //     fprintf(stderr, "[LEXICAL ERROR]: unsuccessful memory allocation\n");
    //     return ERROR_INTERNAL;
    // }
    
    //Сheck if allocation was successful.
    // if (string_init(String) == false)
    // {
    //     //Error handling. 
    //     fprintf(stderr, "[LEXICAL ERROR]: unsuccessful memory allocation\n");
    //     return ERROR_INTERNAL;
    // }
    //string_free(String);
    str_clear(String);
    Token->String = String;   
    
    int current_state = STATE_START; 
    int sign;                 //Sign which is taken one by one from the input string.
    bool is_sign = false;     //For STATE_NUMBER case.
    bool is_exponent = false; //For STATE_NUMBER_DOUBLE case.
    char ascii_number[3];     //For working with numbers in escape sequences.
    
    while(true)
    {
        sign = getchar_modified();
        switch(current_state)
        {
            //Handling a situation with a coincidence of signs.
            case(STATE_START):

                if (sign == '\n')
                {
                    Token->type_of_token = token_EOL;
                    //string_free(String);
                    return 0;
                }


                if (sign == '_' || isalpha(sign))
                {
                    str_add_sign(String, sign);
                    current_state = STATE_ID_OR_KEYWORD; 
                    break;
                }
                

                if (isdigit(sign))
                {
                    str_add_sign(String, sign);
                    current_state = STATE_NUMBER;
                    break;
                }


                if (sign == '+')
                {
                    Token->type_of_token = token_plus;
                    //string_free(String);
                    return 0;
                }


                if (sign == '*')
                {
                    Token->type_of_token = token_multiply;
                    //string_free(String);
                    return 0;
                }


                if (sign == '*')
                {
                    Token->type_of_token = token_multiply;
                    //string_free(String);
                    return 0;
                }
                

                if (sign == '(')
                {
                    Token->type_of_token = token_left_bracket;
                    //string_free(String);
                    return 0;
                }


                if (sign == ')')
                {
                    Token->type_of_token = token_right_bracket;
                    //string_free(String);
                    return 0;
                }


                if (sign == ',')
                {
                    Token->type_of_token = token_comma;
                    //string_free(String);
                    return 0;
                }


                if (sign == ':')
                {
                    Token->type_of_token = token_colon;
                    //string_free(String);
                    return 0;
                }
                

                if (sign == '#')
                {
                    Token->type_of_token = token_length;
                    //string_free(String);
                    return 0;
                }
                    

                if (sign == '.')
                {
                    current_state = STATE_CONCATENATION_START;
                    break;
                }
                

                if (sign == '~')
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


                if (sign == '-')
                {
                    current_state = STATE_MINUS;
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
                    Token->type_of_token = token_EOF;
                    //string_free(String);
                    return 0;
                }

                //If there were no signs matches, then a lexical error is written out.
                fprintf(stderr, "[LEXICAL ERROR]:%d:%d: invalid sign: \"%c\"\n", line_counter, sign_counter, sign);
                //string_free(String);
                return ERROR_LEXICAL;
                                    
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
                             sign == '=' || sign == '~' || sign == '(' || sign == ')' || sign == ',')
                        break;
                    else if (!isdigit(sign))
                    {
                        //Error handling. 
                        str_add_sign(String, sign);
                        fprintf(stderr, "[LEXICAL ERROR]:%d:%d: wrong form of integer: \"%s\"\n", line_counter, sign_counter, Token->String->ptr);
                        //string_free(String);
                        return ERROR_LEXICAL; 
                    }
                    
                    sign = getchar_modified();
                }    

                if (is_sign == true)
                    break;

                //Converts string to int. 
                Token->integer = atoi(String->ptr);
                Token->type_of_token = token_integer;
                current_state = STATE_START;
                ungetc(sign, s_fptr);
                //string_free(String);
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
                    fprintf(stderr, "[LEXICAL ERROR]:%d:%d: wrong form of exponent: \"%s\"\n", line_counter, sign_counter, Token->String->ptr);
                    //string_free(String);
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
                    fprintf(stderr, "[LEXICAL ERROR]:%d:%d: wrong form of exponent: \"%s\"\n", line_counter, sign_counter, Token->String->ptr);
                    //string_free(String);
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
                        fprintf(stderr, "[LEXICAL ERROR]:%d:%d: wrong form of exponent: \"%s\"\n", line_counter, sign_counter, Token->String->ptr);
                        //string_free(String);
                        return ERROR_LEXICAL; 
                    }
                         
                    sign = getchar_modified();                      
                }
                Token->type_of_token = token_exponent;
                //Converts string to a floating-point number (decimal).
                Token->decimal = atof(String->ptr);
                ungetc(sign, s_fptr);
                //string_free(String);
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
                    fprintf(stderr, "[LEXICAL ERROR]:%d:%d: wrong form of decimal: \"%s\"\n", line_counter, sign_counter, Token->String->ptr);
                    //string_free(String);
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
                    fprintf(stderr, "[LEXICAL ERROR]:%d:%d: wrong form of decimal: \"%s\"\n", line_counter, sign_counter, Token->String->ptr);
                    //string_free(String);
                    return ERROR_LEXICAL; 
                }
                
                sign = getchar_modified();
            }        
            if (is_exponent == true) break;
            Token->decimal = atof(String->ptr);
            Token->type_of_token = token_double;

            current_state = STATE_START;
            ungetc(sign, s_fptr);
            //string_free(String);
            return 0;
            
            //Handling a situation with division sign '/'.
            case(STATE_DIVISION):
                if (sign == '/')
                    current_state = STATE_DIVISION_INTEGER;
                else
                    {
                        ungetc(sign, s_fptr);
                        Token->type_of_token = token_divide;
                        //string_free(String);
                        return 0;
                    }
                break;

            //Handling a situation with division integer "//".
            case(STATE_DIVISION_INTEGER):
                Token->type_of_token = token_divide_integer;
                ungetc(sign, s_fptr);
                //string_free(String);
                return 0;
            
            //Handling a situation with relation operator '<'.
            case(STATE_LESS):
                if (sign == '=')
                    current_state = STATE_LESS_OR_EQUAL;
                else
                    {
                        ungetc(sign, s_fptr);
                        Token->type_of_token = token_less;
                        //string_free(String);
                        return 0;
                    }
                break;
            
            //Handling a situation with relation operator "<=".
            case(STATE_LESS_OR_EQUAL):
                Token->type_of_token = token_less_or_equal;
                ungetc(sign, s_fptr);
                //string_free(String);
                return 0;

            
            //Handling a situation with relation operator '>'.
            case(STATE_GREATER):
                if (sign == '=')
                    current_state = STATE_GREATER_OR_EQUAL;
                else
                    {
                        ungetc(sign, s_fptr);
                        Token->type_of_token = token_greater;
                        //string_free(String);
                        return 0;
                    }
                break;

            //Handling a situation with relation operator ">=".
            case(STATE_GREATER_OR_EQUAL):
                Token->type_of_token = token_greater_or_equal;
                ungetc(sign, s_fptr);
                //string_free(String);
                return 0;


            //Handling a situation with equal sign '='.
            case(STATE_EQUAL_SIGN):
                if (sign == '=')
                    current_state = STATE_IS_EQUAL;
                else
                    {
                        ungetc(sign, s_fptr);
                        Token->type_of_token = token_equal_sign; 
                        //string_free(String);
                        return 0;
                    }
                break;
            
            //Handling a situation with relation operator "==".
            case(STATE_IS_EQUAL):
                Token->type_of_token = token_equal; 
                ungetc(sign, s_fptr);
                //string_free(String);
                return 0;
            
            //Handling a situation with start of relation operator "~=".
            case(STATE_NOT_EQUAL_START):
                if (sign == '=')
                    current_state = STATE_NOT_EQUAL;
                else
                {
                    str_add_sign(String,'~');
                    str_add_sign(String,sign);
                    fprintf(stderr, "[LEXICAL ERROR]:%d:%d: unsuitable combination of characters: \"%s\"\n", line_counter, sign_counter, Token->String->ptr);
                    //string_free(String);
                    return ERROR_LEXICAL;
                }
                break;
            
            //Handling a situation with relation operator "~=".
            case(STATE_NOT_EQUAL):
                Token->type_of_token = token_not_equal;
                ungetc(sign, s_fptr);
                //string_free(String);
                return 0;

            //Handling a situation with minus sign '-'.
            case(STATE_MINUS):
                if (sign == '-') 
                    current_state = STATE_COMMENT;
                else 
                {
                    ungetc(sign, s_fptr);
                    Token->type_of_token = token_minus;
                    //string_free(String);
                    return 0; 
                }
                break;

            //Handling a situation with line comment "--".
            case(STATE_COMMENT):
                if (sign == '[')
                {
                    current_state = STATE_COMMENT_BLOCK_START;
                    break;
                }
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

            //Handling a situation with start of block comment "--[".
            case(STATE_COMMENT_BLOCK_START):
                if (sign == '[')
                    current_state = STATE_COMMENT_BLOCK_CONTINUE;
                else
                    current_state = STATE_COMMENT;
                break;

            //Handling a situation with block comment "--[[".
            case(STATE_COMMENT_BLOCK_CONTINUE):
                while(true)
                {
                    if (sign == EOF)
                    {
                        current_state = STATE_START;
                        ungetc(sign, s_fptr);
                        break;
                    }
                    else if (sign != ']') 
                        sign = getchar_modified();
                    else
                    {
                        current_state = STATE_COMMENT_BLOCK_END;
                        break;
                    }
                }
                break;

            //Handling a situation with end of block comment "--[[]]".
            case(STATE_COMMENT_BLOCK_END):
                if (sign == ']')
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
                        fprintf(stderr, "[LEXICAL ERROR]:%d:%d: no end of string\n", line_counter, sign_counter);
                        //string_free(String);
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
                else if (sign == '0' || sign == '1')
                {
                    current_state = STATE_STRING_BACKSLASH_ZERO_TO_ONE;
                    ascii_number[0] = sign; 
                    break;
                }
                else if (sign == '2')
                {
                    current_state = STATE_STRING_BACKSLASH_TWO;   
                    ascii_number[0] = sign; 
                    break;
                }
                else 
                {
                    str_add_sign(String, '\\');
                    str_add_sign(String, sign);
                    fprintf(stderr, "[LEXICAL ERROR]:%d:%d: invalid escape sequence: \"%s\"\n", line_counter, sign_counter, Token->String->ptr);
                    //string_free(String);
                    return ERROR_LEXICAL;
                }
                    
            //Handling a situation with escape sequence ascii[x][][].
            case(STATE_STRING_BACKSLASH_ZERO_TO_ONE):
                if (sign >= '0' && sign <= '9')
                {
                    current_state = STATE_STRING_BACKSLASH_ZERO_TO_NINE;
                    ascii_number[1] = sign;
                    break;
                }
                else 
                {
                    fprintf(stderr, "[LEXICAL ERROR]:%d:%d: invalid escape sequence\n", line_counter, sign_counter);
                    //string_free(String);
                    return ERROR_LEXICAL;
                }
                    

            //Handling a situation with escape sequence ascii[x][y][].
            case(STATE_STRING_BACKSLASH_ZERO_TO_NINE):
                if (sign >= '0' && sign <= '9')
                {   
                    //[x][y][z].
                    current_state = STATE_STRING_START;
                    ascii_number[2] = sign;
                    str_add_sign(String, atoi(ascii_number));
                    break;
                }
                else 
                {
                    fprintf(stderr, "[LEXICAL ERROR]:%d:%d: invalid escape sequence\n", line_counter, sign_counter);
                    //string_free(String);
                    return ERROR_LEXICAL;
                }

            //Handling a situation with escape sequence ascii[x][][].
            case(STATE_STRING_BACKSLASH_TWO):
                if (sign >= '0' && sign <= '4')
                {
                    current_state = STATE_STRING_BACKSLASH_ZERO_TO_NINE;
                    ascii_number[1] = sign;
                    break;
                }
                if (sign == '5')
                {
                    current_state = STATE_STRING_BACKSLASH_FIVE;
                    ascii_number[1] = sign;
                    break;
                }
                else 
                {
                    fprintf(stderr, "[LEXICAL ERROR]:%d:%d: invalid escape sequence\n", line_counter, sign_counter);
                    //string_free(String);
                    return ERROR_LEXICAL;
                }
                
            //Handling a situation with escape sequence ascii[x][y][].
            case(STATE_STRING_BACKSLASH_FIVE):
                if (sign >= '0' && sign <= '5')
                {
                    //[x][y][z].
                    current_state = STATE_STRING_START;
                    ascii_number[2] = sign;
                    str_add_sign(String, atoi(ascii_number));
                    break;
                }
                else 
                {
                    fprintf(stderr, "[LEXICAL ERROR]:%d:%d: invalid escape sequence\n", line_counter, sign_counter);
                    //string_free(String);
                    return ERROR_LEXICAL;
                }

            //Handling a situation with end of string '"'.
            case(STATE_STRING):
                Token->type_of_token = token_string;
                current_state = STATE_START;
                ungetc(sign, s_fptr);
                
                return 0;

            //Handling a situation with start of concatenation "..".
            case(STATE_CONCATENATION_START):
                if (sign == '.')
                    current_state = STATE_CONCATENATION;
                else
                {
                    str_add_sign(String,'.');
                    str_add_sign(String,sign);
                    fprintf(stderr, "[LEXICAL ERROR]:%d:%d: unsuitable combination of characters: \"%s\"\n", line_counter, sign_counter, Token->String->ptr);
                    //string_free(String);
                    return ERROR_LEXICAL;
                }
                break;

            //Handling a situation with concatenation "..".
            case(STATE_CONCATENATION):
                Token->type_of_token = token_concatination;
                //string_free(String);
                return 0;
        }
    }
}
