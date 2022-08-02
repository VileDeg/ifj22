#include "base.h"
#include "debug.h"

int main(int argc, char** argv)
{
    scanner_init();
    
    lexical_test("../tests/lex/characters/amp.tl");
    lexical_test("../tests/lex/characters/bracket.tl");
    lexical_test("../tests/lex/characters/bracket2.tl");

    scanner_terminate();
    return 0;
}
