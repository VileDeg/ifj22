# IFJ22
## Clone
To clone the repo type
```sh
git clone https://username@github.com/VileDeg/ifj21
```
where username is your github username.
## Source files
- base.h     - includes all used standard library headers and declared utility functions/macros. Should be included in every header file.
- debug.h    - declares functions for testing/debugging.
- scanner.h  - scanner.
- string_t.h - dynamic string class.
- vector_t.h - generic vector class.
- stack_t.h  - generic stack class.
- main.c     - program entry point.
## Build and run
```sh
make ; out/ifj22
```
or
```sh
make clean ; make ; out/ifj22
```
## Testing
Folder tests has some examples of "ifj22" language.
Example of a program to test the scanner:
```c
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
```
## Generic data structures
- vector_t.h - basically a dynamic size array. Implemented with a heap-allocated array.
- stack_t.h - stack data structure. Implemented with a heap-allocated array.
### Vector
To generate a vector data structure definition use "GENERATE_VECTOR_DEFINITION" macro.
```c
GENERATE_VECTOR_DEFINITION(int, i)   //generates 'veci' type that corresponds to vector holding integer items.
GENERATE_VECTOR_DEFINITION(float, f) //generates 'vecf' type that corresponds to vector holding float items.
```
There is also a utility macro "iterate" that lets you iterate through all items of a vector.
```c
#define iterate(i, vec)\
    for (size_t i = 0; i < vec->size; ++i)
```
### Stack
To generate a stack data structure definition use "GENERATE_STACK_DEFINITION" macro.
```c
GENERATE_STACK_DEFINITION(int, i)   //generates 'stacki' type that corresponds to stack holding integer items.
GENERATE_STACK_DEFINITION(float, f) //generates 'stackf' type that corresponds to stack holding float items.
```
### Testing example
```c
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
    veci* vi = veci_const();

    for (int i = 0; i < 17; ++i)
    {
        veci_push_back(vi, i);
    }

    veci_print(vi);
    int sumi = 0;

    iterate(i, vi)
    {
        sumi += veci_get(vi, i);
    }
    printf("Sum is: %d\n", sumi);

    veci_dest(vi);

    vecf* vf = vecf_const();

    for (int i = 0; i < 17; ++i)
    {
        vecf_push_back(vf, i*1.1f);
    }

    vecf_print(vf);
    int sumf = 0;

    iterate(i, vf)
    {
        sumf += vecf_get(vf, i);
    }
    printf("Sum is: %d\n", sumf);

    vecf_dest(vf);
    stacki* sti = stacki_const();

    stacki_push(sti, 1);
    stacki_push(sti, 14);
    stacki_push(sti, 7);
    stacki_push(sti, -5);

    stacki_print(sti);

    stacki_dest(sti);

    stackf* stf = stackf_const();

    stackf_push(stf, 5.5f);
    stackf_push(stf, 11.34f);
    stackf_push(stf, 0.f);
    stackf_push(stf, -5666.42f);

    stackf_print(stf);

    stackf_dest(stf);
    return 0;
}

```
