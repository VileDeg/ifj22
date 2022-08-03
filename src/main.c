#include "base.h"
#include "debug.h"
#include "stack_t.h"

GENERATE_STACK(int, i)
GENERATE_STACK(float, f)

int main(int argc, char** argv)
{
    stack_i* sti = stack_const_i();

    stack_push_i(sti, 1);
    stack_push_i(sti, 14);
    stack_push_i(sti, 7);
    stack_push_i(sti, -5);

    stack_print_i(sti);

    stack_dest_i(sti);

    stack_f* stf = stack_const_f();

    stack_push_f(stf, 5.5f);
    stack_push_f(stf, 11.34f);
    stack_push_f(stf, 0.f);
    stack_push_f(stf, -5666.42f);

    stack_print_f(stf);

    stack_dest_f(stf);

    // scanner_init();

    // lexical_test("../tests/lex/characters/amp.tl");
    // lexical_test("../tests/lex/characters/bracket.tl");
    // lexical_test("../tests/lex/characters/bracket2.tl");

    // scanner_terminate();
    return 0;
}
