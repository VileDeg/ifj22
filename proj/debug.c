#include <stdlib.h>
#include <string.h>

#include "debug.h"
#include "scanner.h"
#include "parser.h"
#include "errors.h"
#include "codegen.h"

#define DEBUGPR(...) do { VILE_ASSERT(g_DebugOut != NULL, "Output file not found");\
    fprintf(g_DebugOut, __VA_ARGS__); fflush(g_DebugOut); } while(0)

#define EXPRDBGPR(...) do {\
    set_debug_out(get_expr_out());\
    DEBUGPR(__VA_ARGS__);\
} while(0)

bool g_DebugOn = false;
FILE* g_DebugOut = NULL;

static FILE* s_ScanOut  = NULL;
static FILE* s_ParsOut  = NULL;
static FILE* s_ExprOut  = NULL;

void set_debug_out(FILE* fptr) { g_DebugOut = fptr; }
FILE* get_scan_out() { return s_ScanOut; }
FILE* get_pars_out() { return s_ParsOut; }
FILE* get_expr_out() { return s_ExprOut; }

static int64_t max_kw = 10;
static const char* kw_str[] = {
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

static int64_t max_kw_type = 30;
static const char* tk_types_str[] = {
    "EOF                ",
    "ID                 ",
    "keyword            ",
    "integer            ",
    "double             ",
    "string             ",
    "null               ",
    "minus              ",
    "plus               ",
    "multiply           ",
    "divide             ",
    "less               ",
    "less_or_equal      ",
    "greater            ",
    "greater_or_equal   ",
    "equal_sign         ",
    "equal              ",
    "not_equal          ",
    "left_bracket       ",
    "right_bracket      ",
    "comma              ",
    "colon              ",
    "semicolon          ",
    "left_curly_bracket ",
    "right_curly_bracket",
    "dot                ",
    "prolog             ",
    "end                "
};

const char* debug_kw(Keyword kw)
{
    return (int64_t)kw < max_kw ? kw_str[kw] : "**NIL**";
}

const char* debug_tk_type(Token_type tt)
{
   return (int64_t)tt < max_kw_type ? tk_types_str[tt] : "**NIL**";
}

static const char* s_TokenDebugFormat = "%-4.4s %-4.4s %-12.12s %-12.12s %-16.16s\n";

void debug_print_token(Token tk)
{
    size_t mxlen = 64;
    char str[mxlen];
    memset(str, '\0', mxlen);
    switch (tk.type)
    {
        case token_integer:
            snprintf(str, mxlen, "%d", tk.integer);
            
            DEBUGPR(s_TokenDebugFormat, str, "",
                "", "", debug_tk_type(tk.type));
            break;
        case token_float:
            snprintf(str, mxlen, "%g", tk.decimal);
            DEBUGPR(s_TokenDebugFormat, "", str,
                "", "", debug_tk_type(tk.type));
            break;
        case token_null:
        case token_keyword:
            DEBUGPR(s_TokenDebugFormat, "", "",
                "", debug_kw(tk.keyword), debug_tk_type(tk.type));
            break;
        default:
            VILE_ASSERT(tk.string.ptr != NULL, "");
            DEBUGPR(s_TokenDebugFormat, "", "",
                tk.string.ptr, "", debug_tk_type(tk.type));
    }
}

#define SEP_LINE "----" "----" "----" "----" "----"
#define VSPACE   "\n\n"
#define SEP_BEG SEP_LINE
#define SEP_END SEP_LINE VSPACE
#define HEADER(txt) DEBUGPR(SEP_BEG txt SEP_END)

void print_file_contents(FILE* src)
{
    HEADER("File contents: ");
    uint64_t maxlen = 1024;
    char line[maxlen];
    while(fgets(line, maxlen, src)!= NULL)
        DEBUGPR("%s", line);

    VILE_ASSERT(src != stdin, "");
    rewind(src);
    DEBUGPR(VSPACE "\n");
}

void debug_setup(FILE* source, bool show_source_contents, 
    FILE* scan_out, FILE* pars_out, FILE* expr_out, FILE* codegen_out)
{
    if (!scan_out && !pars_out)
        return;
    
    g_DebugOn = true;

    s_ScanOut = scan_out;
    s_ParsOut = pars_out;
    s_ExprOut = expr_out;
    set_codegen_out(codegen_out);

    if (show_source_contents && source != stdin)
    {
        if (scan_out)
        {
            set_debug_out(scan_out);
            print_file_contents(source);    
        }
        if (pars_out && scan_out != pars_out)
        {
            set_debug_out(pars_out);
            print_file_contents(source);
        }
    }

    if (pars_out)
    {
        set_debug_out(pars_out);
        HEADER("Rules triggered: ");
    }

    if (scan_out)
    {
        set_debug_out(scan_out);
        HEADER("List of tokens: ");
        DEBUGPR(s_TokenDebugFormat, "int", "deci", "string", "keyword", "type");
    }
}

void debug_terminate(FILE* scan_out, FILE* pars_out)
{
    if (pars_out)
    {
        set_debug_out(pars_out);
        DEBUGPR(VSPACE);
    }
    if (scan_out)
    {
        set_debug_out(scan_out);
        DEBUGPR(VSPACE);
    }
}

static const char* s_RulesFilepath = "../LL-grammar.txt";

#define RULE_EXP_MXLEN 256
#define NUM_RULES 35

static struct {
    char rule_name [NUM_RULES][RULE_EXP_MXLEN];
    char exp_string[NUM_RULES][RULE_EXP_MXLEN];
} RuleInfo;

void populate_rule_definitions()
{
    FILE* fptr = fopen(s_RulesFilepath, "r");
    VILE_ASSERT(fptr, "");

    for (uint64_t rule = 0; rule < NUM_RULES; rule++)
    {
        int64_t c = fgetc(fptr); //skip <
        uint64_t letter = 0;
        //Read rule name
        while ((c = fgetc(fptr)) != '>')
        {
            RuleInfo.rule_name[rule][letter] = c;
            ++letter;
            VILE_ASSERT(letter < RULE_EXP_MXLEN, "");
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
            VILE_ASSERT(letter < RULE_EXP_MXLEN, "");
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
    for (uint64_t rule = 0; rule < NUM_RULES; rule++)
    {
        DEBUGPR("%s -> %s\n", RuleInfo.rule_name[rule], RuleInfo.exp_string[rule]);
    }
}

const char* get_rule_expansion_by_name(const char* rulename, int64_t expnum)
{
    for (uint64_t i = 0; i < NUM_RULES; i++)
    {
        if (!strcmp(RuleInfo.rule_name[i], rulename))
        {
            if (expnum == 0)
                return RuleInfo.exp_string[i];
            --expnum;
        }
    }
    VILE_ASSERT(false, "Rule not found.");
    return NULL;
}
