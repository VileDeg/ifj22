#include "debug.h"
#include "base.h"

static uint32_t max_kw = 10;
static const char* kw_str[] =
{
    "else",
    "float",
    "function",
    "if",
    "int",
    "null",
    "return",
    "string",
    "void",
    "while"
};

static int max_kw_type = 31;
static const char* tk_types_str[] = {
    "EOL              ",
    "EOF              ",
    "ID               ",
    "keyword          ",
    "integer          ",
    "double           ",
    "exponent         ",
    "string           ",
    "minus            ",
    "plus             ",
    "multiply         ",
    "divide           ",
    "divide_integer   ",
    "less             ",
    "less_or_equal    ",
    "greater          ",
    "greater_or_equal ",
    "equal_sign       ",
    "equal            ",
    "not_equal        ",
    "left_bracket     ",
    "right_bracket    ",
    "comma            ",
    "colon            ",
    "length           ",
    "concatination    ",
    "question_mark    ",
    "semicolon        ",
    "left_curly_bracket",
    "right_curly_bracket",
    "point            "
};

const char* debug_kw(Keywords kw)
{
    return kw < max_kw ? kw_str[kw] : "**NIL**";
}

const char* debug_tk_type(Token_types tt)
{
   return tt < max_kw_type ? tk_types_str[tt] : "**NIL**";
}
 
//Token now contains union. Don't use this function
void reset_token(Token* tk)
{
    tk->value.integer = 0;
    tk->value.decimal = 0.f;
    tk->value.keyword = 1000;
    tk->type = 1000;
    
    str_clear(tk->value.String);
}

static const char* s_TokenDebugFormat = "%-4.4s %-4.4s %-12.12s %-12.12s %-16.16s\n";

void debug_token(Token tk)
{
    size_t mxlen = 64;
    char str[mxlen];
    memset(str, '\0', mxlen);
    switch (tk.type)
    {
        case token_integer:
            snprintf(str, mxlen, "%d", tk.value.integer);
            printf(s_TokenDebugFormat, str, "",
                "", "", debug_tk_type(tk.type));
            break;
        case token_double:
        case token_exponent:
            snprintf(str, mxlen, "%g", tk.value.decimal);
            printf(s_TokenDebugFormat, "", str,
                "", "", debug_tk_type(tk.type));
            break;
        case token_keyword:
            printf(s_TokenDebugFormat, "", "",
                "", debug_kw(tk.value.keyword), debug_tk_type(tk.type));
            break;
        default:
            ASSERT(tk.value.String != NULL, "");
            ASSERT(tk.value.String->ptr != NULL, "");
            printf(s_TokenDebugFormat, "", "",
                tk.value.String->ptr, "", debug_tk_type(tk.type));
    }
}

void lexical_test(const char* filename)
{
    FILE* fptr = fopen(filename, "r");
    if (fptr == NULL)
    {
        ERRPR("File pointer is null.");
        return;
    }

    printf("***File contents: ***\n");

    char line[1000];
    while(fgets(line, 1000, fptr)!= NULL)
    {
        printf("%s", line);
    }
    if (fptr != stdin)
        rewind(fptr);

    printf("\n");
    printf("***List of tokens: ***\n");
    printf(s_TokenDebugFormat, "int", "deci", "string", "keyword", "type");

    scanner_reset();
    scanner_set_file(fptr);

    Token* tk = s_calloc(sizeof(*tk));

    //reset_token(tk);
    while (!next_token(tk))
    {
        debug_token(*tk);
        //reset_token(tk);
    }

    S_FREE(tk);
    fclose(fptr);
}

