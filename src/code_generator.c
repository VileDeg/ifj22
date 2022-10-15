#include "code_generator.h"

/**
* GENERATOR OF BUILT-IN FUNCTIONS
*/

/// function reads() : ?string
#define FUNCTION_READS              \
        "# Function reads\n"        \
        "LABEL @reads\n"            \
        "PUSHFRAME\n"               \
        "DEFVAR LF@return\n"        \
        "READ LF@return string\n"   \
        "POPFRAME\n"                \
        "RETURN\n"

/// function readi() : ?int
#define FUCNTION_READI          \
        "# Function readi\n"    \
        "LABEL @readi\n"        \
        "PUSHFRAME\n"           \
        "DEFVAR LF@return\n"    \
        "READ LF@return int\n"  \
        "POPFRAME\n"            \
        "RETURN\n"

/// function readf() : ?float
#define FUNCTION_READF              \
        "# Function readf\n"        \
        "LABEL @readf\n"            \
        "PUSHFRAME\n"               \
        "DEFVAR LF@return\n"        \
        "READ LF@return float\n"    \
        "POPFRAME\n"                \
        "RETURN\n"

/// function strlen(string $𝑠) : int
#define FUNCTION_STRLEN                 \
        "# Function strlen\n"           \
        "LABEL @strlen\n"               \
        "PUSHFRAME\n"                   \
        "DEFVAR LF@return\n"            \
        "STRLEN LF@return LF@-0\n"      \
        "POPFRAME\n"                    \
        "RETURN\n"

/// function substring(string $𝑠, int $𝑖, int $𝑗) : ?string
#define FUNCTION_SUBSTRING                              \
        "# Function substring\n"                        \
        "LABEL @substring\n"                            \
        "PUSHFRAME\n"                                   \
        "DEFVAR LF@return\n"                            \
        "MOVE LF@return string@\n"                      \
        "DEFVAR LF@length\n"                            \
        "CREATEFRAME\n"                                 \
        "DEFVAR TF@-0\n"                                \
        "MOVE TF@-0 LF@-0\n"                            \
        "CALL @strlen\n"                                \
        "MOVE LF@length TF@return\n"                    \
        "DEFVAR LF@cond\n"                              \
        "LT LF@cond LF@-1 int@0\n"                      \
        "JUMPIFEQ @substring_end LF@cond bool@true\n"   \
        "LT LF@cond LF@-2 int@0\n"                      \
        "JUMPIFEQ @substring_end LF@cond bool@true\n"   \
        "GT LF@cond LF@-1 LF@-2\n"                      \
        "JUMPIFEQ @substring_end LF@cond bool@true\n"   \
        "LT LF@cond LF@-1 LF@length\n"                  \
        "JUMPIFEQ @substring_end LF@cond bool@false\n"  \
        "GT LF@cond LF@-2 LF@length\n"                  \
        "JUMPIFEQ @substring_end LF@cond bool@true\n"   \
        "DEFVAR LF@index\n"                             \
        "MOVE LF@index LF@-1\n"                         \
        "DEFVAR LF@char\n"                              \
        "LABEL @substring_loop\n"                       \
        "GETCHAR LF@char LF@-0 LF@index\n"              \
        "CONCAT LF@return LF@return LF@char\n"          \
        "ADD LF@index LF@index int@1\n"                 \
        "LT LF@cond LF@index LF@-2\n"                   \
        "JUMPIFEQ @substring_loop LF@cond bool@true\n"  \
        "LABEL @substring_end\n"                        \
        "POPFRAME\n"                                    \
        "RETURN\n"

/// function ord(string $c) : int
#define FUNCTION_ORD                                \
        "# Function ord\n"                          \
        "LABEL @ord\n"                              \
        "PUSHFRAME\n"                               \
        "DEFVAR LF@return\n"                        \
        "MOVE LF@return int@0\n"                    \
        "DEFVAR LF@length\n"                        \
        "CREATEFRAME\n"                             \
        "DEFVAR TF@-0\n"                            \
        "MOVE TF@-0 LF@-0\n"                        \
        "CALL @strlen\n"                            \
        "MOVE LF@length TP@return\n"                \
        "DEFVAR LF@cond\n"                          \
        "GT LF@cond LF@length int@0\n"              \
        "JUMPIFEQ @ord_end LF@cond bool@false\n"    \
        "STRI2INT LF@return LF@-0 int@0\n"          \
        "LABEL @ord_end\n"                          \
        "POPFRAME\n"                                \
        "RETURN\n"

/// function chr(int $i) : string
#define FUNCTION_CHR                            \
        "# Function chr\n"                      \
        "LABEL @chr\n"                          \
        "PUSHFRAME\n"                           \
        "DEFVAR LF@return\n"                    \
        "MOVE LF@return string@\n"              \
        "DEFVAR LF@cond\n"                      \
        "GT LF@cond LF@-0 int@255\n"            \
        "JUMPIFEQ $chr_end LF@cond bool@true\n" \
        "LT LF@cond LF@-0 int@0\n"              \
        "JUMPIFEQ $chr_end LF@cond bool@true\n" \
        "INT2CHAR LF@return LF@-0\n"            \
        "LABEL @chr_end\n"                      \
        "POPFRAME\n"                            \
        "RETURN\n"


str_t code;

bool generate_header() {
    ADD_CODE_N(".IFJcode22");
    ADD_CODE_N("JUMP @main");
    return true;
}

bool generate_built_in_funcs() {
    ADD_CODE(FUNCTION_READS);
    ADD_CODE(FUCNTION_READI);
    ADD_CODE(FUNCTION_READF);
    ADD_CODE(FUNCTION_STRLEN);
    ADD_CODE(FUNCTION_SUBSTRING);
    ADD_CODE(FUNCTION_ORD);
    ADD_CODE(FUNCTION_CHR);
    return true;
}

bool code_generator_start() {
    if (!str_const(&code)) return false;
    if (!generate_header()) return false;
    if (!generate_built_in_funcs()) return false;
    return true;
}

void code_generator_finish() {
    str_dest(&code);
}

void code_generator_flush(FILE* file) {
    fprintf(file, "%s", code.ptr);
    code_generator_finish();
}

bool generate_main_start() {
    ADD_CODE_N("# MAIN FUNCTION\n"
               "LABEL @main\n"
               "CREATEFRAME\n"
               "PUSHFRAME");
    return true;
}

bool generate_main_end() {
    ADD_CODE_N("POPFRAME\n"
               "CLEARS");
    return true;
}