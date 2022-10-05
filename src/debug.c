#include <stdlib.h>
#include <string.h>

#include "debug.h"
#include "scanner.h"
#include "parser.h"
#include "errors.h"

static uint64_t max_kw = 10;
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

static int max_kw_type = 29;
static const char* tk_types_str[] = {
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
    "semicolon        ",
    "left_curly_bracket",
    "right_curly_bracket",
    "dot              ",
    "prolog           ",
    "end"
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
        case token_float:
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

void print_file_contents(FILE* fptr)
{
    printf("***File contents: ***\n");
    uint64_t maxlen = 1024;
    char line[maxlen];
    while(fgets(line, maxlen, fptr)!= NULL)
    {
        printf("%s", line);
    }
    ASSERT(fptr != stdin, "");
    rewind(fptr);
    printf("\n");
}

void lexical_test(const char* filename, bool show_contents)
{
    FILE* fptr = fopen(filename, "r");
    if (fptr == NULL)
    {
        ERRPR("File pointer is null.");
        return;
    }
    str_t* string = calloc(1, sizeof(str_t));
    str_const(string);
    scanner_set_file(fptr);
    scanner_set_string(string);
    if (show_contents)
        print_file_contents(fptr);

    printf("***List of tokens: ***\n");
    printf(s_TokenDebugFormat, "int", "deci", "string", "keyword", "type");

    scanner_reset();
    scanner_set_file(fptr);

    Token* tk = calloc(1, sizeof(*tk));

    //reset_token(tk);
    bool eof = false;
    
    while (!eof)
    {
        eof = scanner_get_next_token(tk);
        debug_token(*tk);
    }
    printf("\n\n");

    scanner_free();
    free(tk);
    fclose(fptr);
}

void parser_test(const char* filename, bool show_contents)
{
    FILE* fptr = fopen(filename, "r");
    if (fptr == NULL)
    {
        ERRPR("File pointer is null.");
        return;
    }
    str_t* string = calloc(1, sizeof(str_t));
    str_const(string);
    scanner_set_file(fptr);
    scanner_set_string(string);
    if (show_contents)
        print_file_contents(fptr);

    printf("\n\n");
    printf("***Rules triggered: ***\n\n");

    int result;
    if ((result = parser_parse()) != 0)
    {
        ERROR_SYNT(" :(");
    }

    printf("\n\n");

    scanner_free();
    
    fclose(fptr);
}

static const char* rules_filepath = "../ifj22-ED-LL-gramatika.txt";
#define rule_exp_maxlen 256
#define numofrules 48
static struct 
{
    char rule_name[numofrules][rule_exp_maxlen];;
    char exp_string[numofrules][rule_exp_maxlen];
} RuleInfo;


void populate_rule_definitions()
{
    FILE* fptr = fopen(rules_filepath, "r");
    uint64_t rule = 0;
    
    for (uint64_t rule = 0; rule < numofrules; rule++)
    {
        fgetc(fptr); //skip <
        int c;
        uint64_t letter = 0;
        //Read rule name
        while ((c = fgetc(fptr)) != '>')
        {
            RuleInfo.rule_name[rule][letter] = c;
            ++letter;
            ASSERT(letter < rule_exp_maxlen, "");
        }
        letter = 0;
        //Skip to after ->
        while (fgetc(fptr) != '>')
            continue;
        //Skip whitespace
        fgetc(fptr);
        //Read rule expansion string
        while (true)
        {
            c = fgetc(fptr);
            if (c == '\n' || c == '\r' || c == EOF)
                break;
            RuleInfo.exp_string[rule][letter] = c;
            ++letter;
            ASSERT(letter < rule_exp_maxlen, "");
        }

        do
        {
            c = fgetc(fptr);
        }
        while (c == '\n' || c == '\r');
        ungetc(c, fptr);
    }

    fclose(fptr);
}

void print_rule_definitions()
{
    for (uint64_t rule = 0; rule < numofrules; rule++)
    {
        printf("%s -> %s\n", RuleInfo.rule_name[rule], RuleInfo.exp_string[rule]);
    }
}

const char* get_rule_expansion_by_name(const char* rulename, int expnum)
{
    for (uint64_t i = 0; i < numofrules; i++)
    {
        if (!strcmp(RuleInfo.rule_name[i], rulename))
        {
            if (expnum == 0)
                return RuleInfo.exp_string[i];
            --expnum;
        }
    }
    ASSERT(false, "Rule not found.");
}
