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
        "JUMPIFNEQ @ord_end LF@length bool@true\n"  \
        "STRI2INT LF@return LF@-0 int@0\n"          \
        "LABEL @ord_end\n"                          \
        "POPFRAME\n"                                \
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