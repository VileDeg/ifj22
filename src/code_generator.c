/**
* GENERATOR OF BUILT-IN FUNCTIONS
*/

/// function strlen(string $𝑠) : int
#define FUNCTION_STRLEN                 \
        "# Function strlen\n"           \
        "LABEL @strlen\n"               \
        "PUSHFRAME\n"                   \
        "DEFVAR LF@return\n"            \
        "STRLEN LF@return LF@-0\n"      \
        "POPFRAME\n"                    \
        "RETURN\n"

/// function chr(int $i) : string
#define FUNCTION_CHR \
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
        "LABEL @chr_end"                        \
        "POPFRAME\n"                            \
        "RETURN\n"