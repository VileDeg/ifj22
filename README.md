# IFJ21
## Source files
- base.h - includes all used standard library headers and declared utility functions/macros. Should be included in every header file.
- debug.h - declares functions for testing/debugging.
- scanner.h - scanner.
- string_t.h - dynamic string class.
- main.c - program entry point.
## Build and run
```sh
make ; out/ifj21
```
or
```sh
make clean ; make ; out/ifj21
```
## Testing
Folder tests has some examples of "ifj21" language.
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
