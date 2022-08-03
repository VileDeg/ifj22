#include "base.h"
#include "debug.h"
#include "stack_t.h"

int main(int argc, char** argv)
{
    stack_t* st = stack_const();
    stack_push(st, 1);
    stack_push(st, 14);
    stack_push(st, 7);
    stack_push(st, -5);

    stack_print_int(st);

    stack_dest(st);

    // scanner_init();

    // lexical_test("../tests/lex/characters/amp.tl");
    // lexical_test("../tests/lex/characters/bracket.tl");
    // lexical_test("../tests/lex/characters/bracket2.tl");

    // scanner_terminate();
    return 0;
}
