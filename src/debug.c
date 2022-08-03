#include "debug.h"
#include "base.h"

static int max_kw = 14;
static const char* kw_str[] =
{
    "do",
    "else",
    "end",
    "function",
    "global",
    "if",
    "integer",
    "local",
    "nil",
    "number",
    "require",
    "return",
    "string",
    "then",
    "while"
};

static int max_kw_type = 25;
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
    "concatination    "
};

const char* debug_kw(Keywords kw)
{
    return kw < max_kw ? kw_str[kw] : "**NIL**";
}

const char* debug_tk_type(Token_types tt)
{
   return tt < max_kw_type ? tk_types_str[tt] : "**NIL**";
}

void reset_token(Token* tk)
{
    tk->integer = 0;
    tk->decimal = 0.f;
    tk->keyword = 1000;
    tk->type_of_token = 1000;
    str_clear(tk->String);
}

static const char* s_TokenDebugFormat = "%-4.4s %-4.4s %-12.12s %-12.12s %-16.16s\n";

void debug_token(Token tk)
{
    size_t mxlen = 16;
    char istr[mxlen];
    snprintf(istr, mxlen, "%d", tk.integer);
    char fstr[mxlen];
    snprintf(fstr, mxlen, "%g", tk.decimal);
    printf(s_TokenDebugFormat, istr, fstr,
        tk.String->ptr, debug_kw(tk.keyword), debug_tk_type(tk.type_of_token));
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

    reset_token(tk);
    while (!next_token(tk))
    {
        debug_token(*tk);
        reset_token(tk);
    }

    S_FREE(tk);
    fclose(fptr);
}

void stack_print_int(stack_t* st)
{
    for (int i = 0; !stack_empty(st); ++i)
    {
        printf("%d:\t%d\n", i, (int)stack_pop(st));
    }
}