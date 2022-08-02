#include "base.h"
#include "debug.h"

int main(int argc, char** argv)
{
    scanner_init();
    //printf("Hello world!\n");
    Token tk;
    // printf("%d\n", argc);
    // printf("%s\n", argv[0]);
    // printf("%s\n", argv[1]);
    //next_token(&tk);
    
    while (!next_token(&tk))
        debug_token(tk);

    scanner_terminate();
    return 0;
}