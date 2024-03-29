/**
 * Project: Implementation of imperative language IFJ22 compiler.
 * 
 * @author Aleksandr Shevchenko (xshevc01@stud.fit.vutbr.cz)
 * @author Vadim Goncearenco   (xgonce00@stud.fit.vutbr.cz)
 */

#ifndef __BUILTINS_H__
#define __BUILTINS_H__

#define _BFNBEG(_name) "# Function " _name " <builtin> ===================================\n"
#define _BFNEND "\n"

/* BUILT-IN FUNCTIONS CODE MACROS */

// function reads() : ?string
#define FUNCTION_READS              \
	_BFNBEG("reads")\
	"LABEL !reads\n"            \
	"PUSHFRAME\n"               \
	"DEFVAR LF@res\n"           \
	"READ LF@res string\n"      \
	"POPFRAME\n"                \
	"RETURN\n" _BFNEND

// function readi() : ?int
#define FUCNTION_READI          \
	_BFNBEG("readi")\
	"LABEL !readi\n"        \
	"PUSHFRAME\n"           \
	"DEFVAR LF@res\n"       \
	"READ LF@res int\n"     \
	"POPFRAME\n"            \
	"RETURN\n" _BFNEND

// function readf() : ?float
#define FUNCTION_READF              \
	_BFNBEG("readf")\
	"LABEL !readf\n"            \
	"PUSHFRAME\n"               \
	"DEFVAR LF@res\n"           \
	"READ LF@res float\n"       \
	"POPFRAME\n"                \
	"RETURN\n" _BFNEND

// function write ( term1 , term2 , ..., term𝑛 ) : void
#define FUNCTION_WRITE                              \
	_BFNBEG("write")\
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

// function floatval(term) : float
#define FUNCTION_FLOATVAL\
    _BFNBEG("floatval")\
    "LABEL !floatval\n" \
    "PUSHFRAME\n" \
	"\n"\
    "DEFVAR LF@type\n"\
    "DEFVAR LF@res\n"\
	"\n"\
	"MOVE LF@res LF@-0\n"\
    "TYPE LF@type LF@-0\n"\
	"\n"\
    "JUMPIFEQ !floatval_end_err LF@type string@string\n"\
    "JUMPIFEQ !floatval_end LF@type string@float\n"\
    "JUMPIFEQ !floatval_arg_nil LF@type string@nil\n"\
	"JUMPIFEQ !floatval_arg_int LF@type string@int\n"\
	"\n"\
	"JUMP !*sem_error_eight\n"\
	"\n"\
	"LABEL !floatval_arg_int\n" \
    "INT2FLOAT LF@res LF@-0\n"\
    "JUMP !floatval_end\n"\
	"\n"\
    "LABEL !floatval_arg_nil\n" \
	"MOVE LF@res float@0x0.0p+0\n" \
	"JUMP !floatval_end\n"\
	"\n"\
	"LABEL !floatval_end_err\n"\
	"MOVE LF@res nil@nil\n"\
	"\n"\
    "LABEL !floatval_end\n"\
    "POPFRAME\n"\
    "RETURN\n" _BFNEND

// function intval(term) : int
#define FUNCTION_INTVAL\
    _BFNBEG("intval")\
    "LABEL !intval\n" \
    "PUSHFRAME\n" \
	"\n"\
    "DEFVAR LF@type\n"\
    "DEFVAR LF@res\n"\
	"\n"\
	"MOVE LF@res LF@-0\n"\
    "TYPE LF@type LF@-0\n"\
	"\n"\
	"JUMPIFEQ !intval_end_err LF@type string@string\n"\
    "JUMPIFEQ !intval_end LF@type string@int\n"\
    "JUMPIFEQ !intval_arg_nil LF@type string@nil\n"\
    "JUMPIFEQ !intval_arg_float LF@type string@float\n"\
	"\n"\
	"JUMP !*sem_error_eight\n"\
	"\n"\
	"LABEL !intval_arg_float\n"\
    "FLOAT2INT LF@res LF@-0\n"\
    "JUMP !intval_end\n"\
	"\n"\
    "LABEL !intval_arg_nil\n" \
    "MOVE LF@res int@0\n" \
	"JUMP !intval_end\n"\
	"\n"\
	"LABEL !intval_end_err\n"\
	"MOVE LF@res nil@nil\n"\
	"\n"\
    "LABEL !intval_end\n"\
    "POPFRAME\n"\
    "RETURN\n" _BFNEND
// function strval(term) : string
#define FUNCTION_STRVAL\
    _BFNBEG("strval")\
    "LABEL !strval\n" \
    "PUSHFRAME\n" \
	"\n"\
    "DEFVAR LF@type\n"\
    "DEFVAR LF@res\n"\
	"\n"\
	"MOVE LF@res LF@-0\n"\
    "TYPE LF@type LF@-0\n"\
	"\n"\
	"JUMPIFEQ !strval_end_err LF@type string@int\n"\
	"JUMPIFEQ !strval_end_err LF@type string@float\n"\
	"JUMPIFEQ !strval_end LF@type string@string\n"\
    "JUMPIFEQ !strval_arg_nil LF@type string@nil\n"\
	"\n"\
	"JUMP !*sem_error_eight\n"\
	"\n"\
	"LABEL !strval_arg_nil\n"\
    "MOVE LF@res string@\n" \
	"JUMP !strval_end\n"\
	"\n"\
	"LABEL !strval_end_err\n"\
	"MOVE LF@res nil@nil\n"\
	"\n"\
    "LABEL !strval_end\n"\
    "POPFRAME\n"\
    "RETURN\n" _BFNEND

// function strlen(string $𝑠) : int
#define FUNCTION_STRLEN                 \
	_BFNBEG("strlen")\
	"LABEL !strlen\n"               \
	"PUSHFRAME\n"                   \
	"DEFVAR LF@res\n"               \
	"STRLEN LF@res LF@-0\n"         \
	"POPFRAME\n"                    \
	"RETURN\n" _BFNEND

// function substring(string $𝑠, int $𝑖, int $𝑗) : ?string
#define FUNCTION_SUBSTRING                              \
	_BFNBEG("substring")\
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

// function ord(string $c) : int
#define FUNCTION_ORD                                \
	_BFNBEG("ord")\
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

// function chr(int $i) : string
#define FUNCTION_CHR                            \
	_BFNBEG("chr")\
	"LABEL !chr\n"                          \
	"PUSHFRAME\n"                           \
	"DEFVAR LF@res\n"                       \
	"MOVE LF@res string@\n"                 \
	"INT2CHAR LF@res LF@-0\n"               \
	"POPFRAME\n"                            \
	"RETURN\n" _BFNEND

#endif // !__BUILTINS_H__

       