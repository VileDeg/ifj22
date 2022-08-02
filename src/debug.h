#include "stdio.h"
#include "scanner.h"

void debug_kw(Keywords k)
{
    switch (k)
    {
    case keyword_do:
        printf("do");
        break;
    case keyword_else:
        printf("else");
        break;
    case keyword_end:
        printf("end");
        break;
    case keyword_function:
        printf("function");
        break;
    case keyword_global:
        printf("global");
        break;
    case keyword_if:
        printf("if");
        break;
    case keyword_integer:
        printf("integer");
        break;
    case keyword_local:
        printf("local");
        break;
    case keyword_nil:
        printf("nil");
        break;
    case keyword_number:
        printf("number");
        break;
    case keyword_require:
        printf("require");
        break;
    case keyword_return:
        printf("return");
        break;
    case keyword_string:
        printf("string");
        break;
    case keyword_then:
        printf("then");
        break;
    case keyword_while:
        printf("while");
        break;
    
    default:
        printf("***ERROR_KW***");
        break;
    }
}

void debug_token(Token tk)
{
    printf("%-2d %-2.2f %-16s %-10d %d\n", tk.integer, tk.decimal, tk.String->string, tk.keyword, tk.type_of_token);
}