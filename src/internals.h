#ifndef __INTERNALS_H__
#define __INTERNALS_H__

#define _IFLBEXT "*"

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
	"RETURN\n" _BFNEND

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
    "RETURN\n" _BFNEND

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
    "RETURN\n" _BFNEND

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
	"EXIT int@8\n" _BFNEND

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
	"RETURN\n" _BFNEND

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
	"RETURN\n" _BFNEND

#endif // __INTERNALS_H__