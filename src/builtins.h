#ifndef __BUILTINS_H__
#define __BUILTINS_H__

/**
* BUILT-IN FUNCTIONS
*/
#define _BFNEND "\n"

/// function reads() : ?string
#define FUNCTION_READS              \
	"# Function reads\n"        \
	"LABEL !reads\n"            \
	"PUSHFRAME\n"               \
	"DEFVAR LF@res\n"           \
	"READ LF@res string\n"      \
	"POPFRAME\n"                \
	"RETURN\n" _BFNEND

/// function readi() : ?int
#define FUCNTION_READI          \
	"# Function readi\n"    \
	"LABEL !readi\n"        \
	"PUSHFRAME\n"           \
	"DEFVAR LF@res\n"       \
	"READ LF@res int\n"     \
	"POPFRAME\n"            \
	"RETURN\n" _BFNEND

/// function readf() : ?float
#define FUNCTION_READF              \
	"# Function readf\n"        \
	"LABEL !readf\n"            \
	"PUSHFRAME\n"               \
	"DEFVAR LF@res\n"           \
	"READ LF@res float\n"       \
	"POPFRAME\n"                \
	"RETURN\n" _BFNEND

/// function write ( term1 , term2 , ..., term𝑛 ) : void
#define FUNCTION_WRITE                              \
	"# Function write\n"                        \
	"LABEL !write\n"                            \
	"PUSHFRAME\n"                               \
	"DEFVAR LF@to_write\n"                      \
	"DEFVAR LF@type\n"                          \
	"TYPE LF@type LF@arg_count\n"                  \
	"JUMPIFNEQ !write_end LF@type string@int\n" \
	"DEFVAR LF@cond\n"                          \
	"LT LF@cond LF@arg_count int@1\n"              \
	"JUMPIFEQ !write_end LF@cond bool@true\n"   \
	"LABEL !write_loop\n"                       \
	"POPS LF@to_write\n"                          \
	"WRITE LF@to_write\n"                         \
	"SUB LF@arg_count LF@arg_count int@1\n"           \
	"GT LF@cond LF@arg_count int@0\n"              \
	"JUMPIFEQ !write_loop LF@cond bool@true\n"  \
	"LABEL !write_end\n"                        \
	"POPFRAME\n"                                \
	"RETURN\n" _BFNEND

/// function floatval(term) : float
#define FUNCTION_FLOATVAL\
    "# Function floatval\n" \
    "LABEL !floatval\n" \
    "PUSHFRAME\n" \
    "DEFVAR TF@type\n"\
    "TYPE TF@type LF@-0\n"\
    "JUMPIFEQ !floatval_end TF@type string@float\n"\
    "JUMPIFEQ !floatval_end TF@type string@string\n"\
    "DEFVAR LF@res\n"\
    "JUMPIFEQ !floatval_arg_null TF@type string@null\n"\
    "INT2FLOAT LF@res LF@-0\n"\
    "JUMP !floatval_end\n"\
    "LABEL !floatval_arg_null\n" \
    "MOVE LF@res float@0x0.0p+0\n" \
    "LABEL !floatval_end\n"\
    "POPFRAME\n"          \
    "RETURN\n" _BFNEND
/// function intval(term) : int
#define FUNCTION_INTVAL\
    "# Function intval\n" \
    "LABEL !intval\n" \
    "PUSHFRAME\n" \
    "DEFVAR TF@type\n"\
    "TYPE TF@type LF@-0\n"\
    "JUMPIFEQ !intval_end TF@type string@int\n"\
    "JUMPIFEQ !intval_end TF@type string@string\n"\
    "DEFVAR LF@res\n"\
    "JUMPIFEQ !intval_arg_null TF@type string@null\n"\
    "FLOAT2INT LF@res LF@-0\n"\
    "JUMP !intval_end\n"\
    "LABEL !intval_arg_null\n" \
    "MOVE LF@res int@0\n" \
    "LABEL !intval_end\n"\
    "POPFRAME\n"          \
    "RETURN\n" _BFNEND
/// function strval(term) : string
#define FUNCTION_STRVAL\
    "# Function strval\n" \
    "LABEL !strval\n" \
    "PUSHFRAME\n" \
    "DEFVAR TF@type\n"\
    "TYPE TF@type LF@-0\n"\
    "JUMPIFNEQ !strval_end TF@type string@null\n"\
    "DEFVAR LF@res\n"\
    "MOVE LF@res string@\n" \
    "LABEL !strval_end\n"\
    "POPFRAME\n"          \
    "RETURN\n" _BFNEND

/// function strlen(string $𝑠) : int
#define FUNCTION_STRLEN                 \
	"# Function strlen\n"           \
	"LABEL !strlen\n"               \
	"PUSHFRAME\n"                   \
	"DEFVAR LF@res\n"               \
	"STRLEN LF@res LF@-0\n"         \
	"POPFRAME\n"                    \
	"RETURN\n" _BFNEND

/// function substring(string $𝑠, int $𝑖, int $𝑗) : ?string
#define FUNCTION_SUBSTRING                              \
	"# Function substring\n"                        \
	"LABEL !substring\n"                            \
	"PUSHFRAME\n"                                   \
	"DEFVAR LF@res\n"                               \
	"MOVE LF@res string@\n"                         \
	"DEFVAR LF@length\n"                            \
	"CREATEFRAME\n"                                 \
	"DEFVAR TF@-0\n"                                \
	"MOVE TF@-0 LF@-0\n"                            \
	"CALL !strlen\n"                                \
	"MOVE LF@length TF@res\n"                       \
	"DEFVAR LF@cond\n"                              \
	"LT LF@cond LF@-1 int@0\n"                      \
	"JUMPIFEQ !substring_end_null LF@cond bool@true\n"   \
	"LT LF@cond LF@-2 int@0\n"                      \
	"JUMPIFEQ !substring_end_null LF@cond bool@true\n"   \
	"GT LF@cond LF@-1 LF@-2\n"                      \
	"JUMPIFEQ !substring_end_null LF@cond bool@true\n"   \
	"LT LF@cond LF@-1 LF@length\n"                  \
	"JUMPIFEQ !substring_end_null LF@cond bool@false\n"  \
	"GT LF@cond LF@-2 LF@length\n"                  \
	"JUMPIFEQ !substring_end_null LF@cond bool@true\n"   \
	"DEFVAR LF@index\n"                             \
	"MOVE LF@index LF@-1\n"                         \
	"DEFVAR LF@char\n"                              \
	"LABEL !substring_loop\n"                       \
	"LT LF@cond LF@index LF@-2\n"                   \
	"JUMPIFEQ !substring_end LF@cond bool@false\n"  \
	"GETCHAR LF@char LF@-0 LF@index\n"              \
	"CONCAT LF@res LF@res LF@char\n"                \
	"ADD LF@index LF@index int@1\n"                 \
	"JUMP !substring_loop\n"                         \
	"LABEL !substring_end_null\n"\
	"MOVE LF@res nil@nil\n"                         \
	"LABEL !substring_end\n"                        \
	"POPFRAME\n"                                    \
	"RETURN\n" _BFNEND

/// function ord(string $c) : int
#define FUNCTION_ORD                                \
	"# Function ord\n"                          \
	"LABEL !ord\n"                              \
	"PUSHFRAME\n"                               \
	"DEFVAR LF@res\n"                           \
	"MOVE LF@res int@0\n"                       \
	"DEFVAR LF@length\n"                        \
	"CREATEFRAME\n"                             \
	"DEFVAR TF@-0\n"                            \
	"MOVE TF@-0 LF@-0\n"                        \
	"CALL !strlen\n"                            \
	"MOVE LF@length TF@res\n"                   \
	"DEFVAR LF@cond\n"                          \
	"GT LF@cond LF@length int@0\n"              \
	"JUMPIFEQ !ord_end LF@cond bool@false\n"    \
	"STRI2INT LF@res LF@-0 int@0\n"             \
	"LABEL !ord_end\n"                          \
	"POPFRAME\n"                                \
	"RETURN\n" _BFNEND

/// function chr(int $i) : string
#define FUNCTION_CHR                            \
	"# Function chr\n"                      \
	"LABEL !chr\n"                          \
	"PUSHFRAME\n"                           \
	"DEFVAR LF@res\n"                       \
	"MOVE LF@res string@\n"                 \
	"INT2CHAR LF@res LF@-0\n"               \
	"POPFRAME\n"                            \
	"RETURN\n" _BFNEND

#endif // !__BUILTINS_H__

//from chr:
	//"DEFVAR LF@cond\n"                      \
	// "GT LF@cond LF@-0 int@255\n"            \
	// "JUMPIFEQ !chr_end LF@cond bool@true\n" \
	// "LT LF@cond LF@-0 int@0\n"              \
	// "JUMPIFEQ !chr_end LF@cond bool@true\n" \

	//"LABEL !chr_end\n"                      