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
#define FUNCTION_SUBSTRING \

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
        "LABEL @chr_end\n"                      \
        "POPFRAME\n"                            \
        "RETURN\n"