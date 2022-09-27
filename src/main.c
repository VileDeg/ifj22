#include "base.h"
#include "debug.h"

int main(void)
{
    scanner_init();

    lexical_test("../tests/lex/eol.php", false);
    lexical_test("../tests/lex/string.php", false);
    lexical_test("../tests/lex/factorial.php", false);
    lexical_test("../tests/lex/factorial_rek.php", false);

    scanner_terminate();
    return 0;
}
