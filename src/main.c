#include "base.h"
#include "debug.h"
#include "stack_t.h"
#include "vector_t.h"

GENERATE_STACK_DEFINITION(int, i)
GENERATE_STACK_DEFINITION(float, f)

GENERATE_VECTOR_DEFINITION(int, i)
GENERATE_VECTOR_DEFINITION(float, f)

int main(void)
{
    // veci* vi = veci_const();

    // for (int i = 0; i < 17; ++i)
    // {
    //     veci_push_back(vi, i);
    // }

    // veci_print(vi);
    // int sumi = 0;

    // iterate(i, vi)
    // {
    //     sumi += veci_get(vi, i);
    // }
    // printf("Sum is: %d\n", sumi);

    // veci_dest(vi);

    // vecf* vf = vecf_const();

    // for (int i = 0; i < 17; ++i)
    // {
    //     vecf_push_back(vf, i*1.1f);
    // }

    // vecf_print(vf);
    // int sumf = 0;

    // iterate(i, vf)
    // {
    //     sumf += vecf_get(vf, i);
    // }
    // printf("Sum is: %d\n", sumf);

    // vecf_dest(vf);
    // stacki* sti = stacki_const();

    // stacki_push(sti, 1);
    // stacki_push(sti, 14);
    // stacki_push(sti, 7);
    // stacki_push(sti, -5);

    // stacki_print(sti);

    // stacki_dest(sti);

    // stackf* stf = stackf_const();

    // stackf_push(stf, 5.5f);
    // stackf_push(stf, 11.34f);
    // stackf_push(stf, 0.f);
    // stackf_push(stf, -5666.42f);

    // stackf_print(stf);

    // stackf_dest(stf);

    scanner_init();

    //lexical_test("../tests/lex/factorial.php");
    //lexical_test("../tests/lex/factorial_rek.php");
    //lexical_test("../tests/lex/string.php");
    lexical_test("../tests/lex/eol.php");

    scanner_terminate();
    return 0;
}
