#ifndef __BUILTINS_H__
#define __BUILTINS_H__

/**
* BUILT-IN FUNCTIONS
*/
#define BFNEND "\n"

#define _RULE_BEGIN_CODE\
	"PUSHFRAME\n"\
	"DEFVAR LF@op1\n"\
    "DEFVAR LF@op2\n"\
    "POPS LF@op2\n"\
    "POPS LF@op1\n"\
    "DEFVAR LF@op1type\n"\
    "DEFVAR LF@op2type\n"\
    "TYPE LF@op1type LF@op1\n"\
    "TYPE LF@op2type LF@op2\n"

#define RULE_ADD_CODE\
	"# Operator '+'\n"\
	"LABEL !rule_add\n"\
	"PUSHFRAME\n"\
	"DEFVAR LF@op1\n"\
	"DEFVAR LF@op2\n"\
	"POPS LF@op2\n"\
	"POPS LF@op1\n"\
	"DEFVAR LF@op1type\n"\
	"DEFVAR LF@op2type\n"\
	"TYPE LF@op1type LF@op1\n"\
	"TYPE LF@op2type LF@op2\n"\
	"JUMPIFEQ !sem_error_seven LF@op1type string@string\n"\
	"JUMPIFEQ !sem_error_seven LF@op2type string@string\n"\
	"JUMPIFEQ !rule_add_op1_nil LF@op1type string@nil\n"\
	"LABEL !rule_add_check_op2_nil\n"\
	"JUMPIFEQ !rule_add_op2_nil LF@op2type string@nil\n"\
	"LABEL !rule_add_check_op1_float\n"\
	"JUMPIFEQ !rule_add_op1_float LF@op1type string@float\n"\
	"JUMPIFEQ !rule_add_op2_float LF@op2type string@float\n"\
	"JUMP !rule_add_end\n"\
	"LABEL !rule_add_op1_nil\n"\
	"	MOVE LF@op1 int@0\n"\
	"	JUMP !rule_add_check_op2_nil\n"\
	"LABEL !rule_add_op2_nil\n"\
	"	MOVE LF@op2 int@0\n"\
	"	JUMP !rule_add_check_op1_float\n"\
	"LABEL !rule_add_op1_float\n"\
	"	JUMPIFEQ !rule_add_op1_float_op2_int LF@op2type string@int\n"\
	"	JUMP !rule_add_end\n"\
	"	LABEL !rule_add_op1_float_op2_int\n"\
	"		INT2FLOAT LF@op2 LF@op2\n"\
	"		JUMP !rule_add_end\n"\
	"LABEL !rule_add_op2_float\n"\
	"	INT2FLOAT LF@op1 LF@op1\n"\
	"	JUMP !rule_add_end\n"\
	"LABEL !rule_add_end\n"\
	"PUSHS LF@op1\n"\
	"PUSHS LF@op2\n"\
	"ADDS\n"\
	"POPFRAME\n"\
	"RETURN\n" BFNEND

#define RULE_EQ_CODE\
	"# Operator '==='\n"\
    "LABEL !rule_eq\n"\
    _RULE_BEGIN_CODE\
    "JUMPIFNEQ !rule_eq_type_eq LF@op1type LF@op2type\n"\
	"PUSHS LF@op1\n"\
	"PUSHS LF@op2\n"\
    "EQS\n"\
    "JUMP !rule_eq_end\n"\
    "LABEL !rule_eq_type_eq\n"\
    "PUSHS bool@false\n"\
    "LABEL !rule_eq_end\n"\
    "POPFRAME\n"\
    "RETURN\n" BFNEND

#define RULE_NEQ_CODE\
	"# Operator '!=='\n"\
    "LABEL !rule_neq\n"\
    _RULE_BEGIN_CODE\
    "JUMPIFEQ !rule_neq_type_eq LF@op1type LF@op2type\n"\
	"PUSHS LF@op1\n"\
	"PUSHS LF@op2\n"\
    "EQS\n"\
    "NOTS\n"\
    "JUMP !rule_neq_end\n"\
    "LABEL !rule_neq_type_eq\n"\
    "PUSHS bool@false\n"\
    "LABEL !rule_neq_end\n"\
    "POPFRAME\n"\
    "RETURN\n" BFNEND

#define ERROR_LABELS_CODE\
	"LABEL !sem_error_three\n"\
	"EXIT int@3\n"\
	"LABEL !sem_error_four\n"\
	"EXIT int@4\n"\
	"LABEL !sem_error_five\n"\
	"EXIT int@5\n"\
	"LABEL !sem_error_six\n"\
	"EXIT int@6\n"\
	"LABEL !sem_error_seven\n"\
	"EXIT int@7\n"\
	"LABEL !sem_error_eight\n"\
	"EXIT int@8\n" BFNEND

#define EXPR_RES_BOOL_CHECK_CODE\
	"# Expression result bool convert\n"\
	"LABEL !expr_res_bool_check\n"\
	"PUSHFRAME\n"\
	"\n"\
	"DEFVAR LF@expr_type\n"\
	"TYPE LF@expr_type GF@EXPR_VAL\n"\
	"\n"\
	"JUMPIFEQ !erbc_end LF@expr_type string@bool\n"\
	"JUMPIFEQ !erbc_type_string LF@expr_type string@string\n"\
	"JUMPIFEQ !erbc_type_int LF@expr_type string@int\n"\
	"JUMPIFEQ !erbc_true LF@expr_type string@float\n"\
	"JUMPIFEQ !erbc_false LF@expr_type string@nil\n"\
	"JUMP !sem_error_seven\n"\
	"\n"\
	"LABEL !erbc_type_string\n"\
	"JUMPIFEQ !erbc_false GF@EXPR_VAL string@\n"\
	"JUMPIFEQ !erbc_false GF@EXPR_VAL string@0\n"\
	"JUMP !erbc_true\n"\
	"\n"\
	"LABEL !erbc_type_int\n"\
	"JUMPIFEQ !erbc_false GF@EXPR_VAL int@0\n"\
	"JUMP !erbc_true\n"\
	"\n"\
	"LABEL !erbc_false\n"\
	"MOVE GF@EXPR_VAL bool@false\n"\
	"JUMP !erbc_end\n"\
	"LABEL !erbc_true\n"\
	"MOVE GF@EXPR_VAL bool@true\n"\
	"LABEL !erbc_end\n"\
	"POPFRAME\n"\
	"RETURN\n" BFNEND

#define RETURN_SEMANTIC_CHECK\
	"# Return semantic check\n"\
	"LABEL !return_semantic_check\n"\
	"PUSHFRAME\n"\
	"DEFVAR LF@func_type\n"\
	"POPS LF@func_type\n"\
	"DEFVAR LF@func_qmark\n"\
	"POPS LF@func_qmark\n"\
	"TYPE GF@EXPR_TYPE GF@EXPR_VAL\n"\
	"JUMPIFEQ !rs_ok GF@EXPR_TYPE LF@func_type\n"\
	"JUMPIFNEQ !sem_error_four LF@func_qmark bool@true\n"\
	"JUMPIFNEQ !sem_error_four GF@EXPR_TYPE string@nil\n"\
	"LABEL !rs_ok\n"\
	"POPFRAME\n"\
	"RETURN\n" BFNEND


/// function reads() : ?string
#define FUNCTION_READS              \
	"# Function reads\n"        \
	"LABEL !reads\n"            \
	"PUSHFRAME\n"               \
	"DEFVAR LF@res\n"           \
	"READ LF@res string\n"      \
	"POPFRAME\n"                \
	"RETURN\n" BFNEND

/// function readi() : ?int
#define FUCNTION_READI          \
	"# Function readi\n"    \
	"LABEL !readi\n"        \
	"PUSHFRAME\n"           \
	"DEFVAR LF@res\n"       \
	"READ LF@res int\n"     \
	"POPFRAME\n"            \
	"RETURN\n" BFNEND

/// function readf() : ?float
#define FUNCTION_READF              \
	"# Function readf\n"        \
	"LABEL !readf\n"            \
	"PUSHFRAME\n"               \
	"DEFVAR LF@res\n"           \
	"READ LF@res float\n"       \
	"POPFRAME\n"                \
	"RETURN\n" BFNEND

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
	"RETURN\n" BFNEND

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
    "RETURN\n" BFNEND
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
    "RETURN\n" BFNEND
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
    "RETURN\n" BFNEND

/// function strlen(string $𝑠) : int
#define FUNCTION_STRLEN                 \
	"# Function strlen\n"           \
	"LABEL !strlen\n"               \
	"PUSHFRAME\n"                   \
	"DEFVAR LF@res\n"               \
	"STRLEN LF@res LF@-0\n"         \
	"POPFRAME\n"                    \
	"RETURN\n" BFNEND

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
	"GETCHAR LF@char LF@-0 LF@index\n"              \
	"CONCAT LF@res LF@res LF@char\n"                \
	"ADD LF@index LF@index int@1\n"                 \
	"LT LF@cond LF@index LF@-2\n"                   \
	"JUMPIFEQ !substring_loop LF@cond bool@true\n"  \
	"JUMP !substring_end\n"                        \
	"LABEL !substring_end_null\n"\
	"MOVE LF@res nil@nil\n"                         \
	"LABEL !substring_end\n"                        \
	"POPFRAME\n"                                    \
	"RETURN\n" BFNEND

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
	"RETURN\n" BFNEND

/// function chr(int $i) : string
#define FUNCTION_CHR                            \
	"# Function chr\n"                      \
	"LABEL !chr\n"                          \
	"PUSHFRAME\n"                           \
	"DEFVAR LF@res\n"                       \
	"MOVE LF@res string@\n"                 \
	"INT2CHAR LF@res LF@-0\n"               \
	"POPFRAME\n"                            \
	"RETURN\n" BFNEND

#endif // !__BUILTINS_H__

//from chr:
	//"DEFVAR LF@cond\n"                      \
	// "GT LF@cond LF@-0 int@255\n"            \
	// "JUMPIFEQ !chr_end LF@cond bool@true\n" \
	// "LT LF@cond LF@-0 int@0\n"              \
	// "JUMPIFEQ !chr_end LF@cond bool@true\n" \

	//"LABEL !chr_end\n"                      