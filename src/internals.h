/**
 * Project: Implementation of imperative language IFJ22 compiler.
 * 
 * @author Vadim Goncearenco    (xgonce00@stud.fit.vutbr.cz)
 * @author Aleksandr Shevchenko (xshevc01@stud.fit.vutbr.cz)
 */

#ifndef __INTERNALS_H__
#define __INTERNALS_H__

#define _IFLBEXT "*"

#define _OPSEP " ===========================\n"

/* INTERNAL FUNCTIONS CODE MACROS */

#define OP_ASM_CODE\
	"# Operators '+', '-', '*'" _OPSEP\
	"LABEL !*op_asm\n"\
	"PUSHFRAME\n"\
	"\n"\
	"DEFVAR LF@op1\n"\
	"DEFVAR LF@op2\n"\
	"DEFVAR LF@op1type\n"\
	"DEFVAR LF@op2type\n"\
	"DEFVAR LF@rule\n"\
	"\n"\
	"POPS LF@rule\n"\
	"POPS LF@op2\n"\
	"POPS LF@op1\n"\
	"\n"\
	"TYPE LF@op1type LF@op1\n"\
	"TYPE LF@op2type LF@op2\n"\
	"\n"\
	"JUMPIFEQ !*sem_error_seven LF@op1type string@string\n"\
	"JUMPIFEQ !*sem_error_seven LF@op2type string@string\n"\
	"\n"\
	"JUMPIFEQ !*op_asm_op1_nil LF@op1type string@nil\n"\
	"LABEL !*op_asm_check_op2_nil\n"\
	"JUMPIFEQ !*op_asm_op2_nil LF@op2type string@nil\n"\
	"\n"\
	"LABEL !*op_asm_check_op1_float\n"\
	"JUMPIFEQ !*op_asm_op1_float LF@op1type string@float\n"\
	"JUMPIFEQ !*op_asm_op2_float LF@op2type string@float\n"\
	"JUMP !*op_asm_do\n"\
	"\n"\
	"LABEL !*op_asm_op1_nil\n"\
	"	MOVE LF@op1 int@0\n"\
	"	JUMP !*op_asm_check_op2_nil\n"\
	"\n"\
	"LABEL !*op_asm_op2_nil\n"\
	"	MOVE LF@op2 int@0\n"\
	"	JUMP !*op_asm_check_op1_float\n"\
	"\n"\
	"LABEL !*op_asm_op1_float\n"\
	"	JUMPIFEQ !*op_asm_op1_float_op2_int LF@op2type string@int\n"\
	"	JUMP !*op_asm_do\n"\
	"\n"\
	"	LABEL !*op_asm_op1_float_op2_int\n"\
	"		INT2FLOAT LF@op2 LF@op2\n"\
	"		JUMP !*op_asm_do\n"\
	"\n"\
	"LABEL !*op_asm_op2_float\n"\
	"	INT2FLOAT LF@op1 LF@op1\n"\
	"	JUMP !*op_asm_do\n"\
	"\n"\
	"LABEL !*op_asm_do\n"\
	"	PUSHS LF@op1\n"\
	"	PUSHS LF@op2\n"\
	"\n"\
	"JUMPIFEQ !*op_asm_add LF@rule string@add\n"\
	"JUMPIFEQ !*op_asm_sub LF@rule string@sub\n"\
	"JUMPIFEQ !*op_asm_mul LF@rule string@mul\n"\
	"\n"\
	"JUMP !*sem_error_eight\n"\
	"\n"\
	"LABEL !*op_asm_add\n"\
	"	ADDS\n"\
	"	JUMP !*op_asm_end\n"\
	"\n"\
	"LABEL !*op_asm_sub\n"\
	"	SUBS\n"\
	"	JUMP !*op_asm_end\n"\
	"\n"\
	"LABEL !*op_asm_mul\n"\
	"	MULS\n"\
	"\n"\
	"LABEL !*op_asm_end\n"\
	"POPFRAME\n"\
	"RETURN\n" _BFNEND

#define OP_DIV_CODE\
	"# Operator '/'" _OPSEP\
	"LABEL !*op_div\n"\
	"PUSHFRAME\n"\
	"\n"\
	"DEFVAR LF@op1\n"\
	"DEFVAR LF@op2\n"\
	"DEFVAR LF@op1type\n"\
	"DEFVAR LF@op2type\n"\
	"\n"\
	"POPS LF@op2\n"\
	"POPS LF@op1\n"\
	"\n"\
	"TYPE LF@op1type LF@op1\n"\
	"TYPE LF@op2type LF@op2\n"\
	"\n"\
	"JUMPIFEQ !*sem_error_seven LF@op1type string@string\n"\
	"JUMPIFEQ !*sem_error_seven LF@op2type string@string\n"\
	"\n"\
	"JUMPIFEQ !*op_div_op1_nil LF@op1type string@nil\n"\
	"LABEL !*op_div_check_op2_nil\n"\
	"JUMPIFEQ !*op_div_op2_nil LF@op2type string@nil\n"\
	"\n"\
	"LABEL !*op_div_check_op1_int\n"\
	"JUMPIFEQ !*op_div_op1_int LF@op1type string@int\n"\
	"LABEL !*op_div_check_op2_int\n"\
	"JUMPIFEQ !*op_div_op2_int LF@op2type string@int\n"\
	"\n"\
	"JUMP !*op_div_do\n"\
	"\n"\
	"LABEL !*op_div_op1_nil\n"\
	"	MOVE LF@op1 int@0\n"\
	"	INT2FLOAT LF@op1 LF@op1\n"\
	"	JUMP !*op_div_check_op2_nil\n"\
	"\n"\
	"LABEL !*op_div_op2_nil\n"\
	"	MOVE LF@op2 int@0\n"\
	"	INT2FLOAT LF@op2 LF@op1\n"\
	"	JUMP !*op_div_check_op1_int\n"\
	"\n"\
	"LABEL !*op_div_op1_int\n"\
	"	INT2FLOAT LF@op1 LF@op1\n"\
	"	JUMP !*op_div_check_op2_int\n"\
	"\n"\
	"LABEL !*op_div_op2_int\n"\
	"	INT2FLOAT LF@op2 LF@op2\n"\
	"	JUMP !*op_div_do\n"\
	"\n"\
	"LABEL !*op_div_do\n"\
	"	PUSHS LF@op1\n"\
	"	PUSHS LF@op2\n"\
	"	DIVS\n"\
	"\n"\
	"LABEL !*op_div_end\n"\
	"POPFRAME\n"\
	"RETURN\n" _BFNEND

#define OP_DOT_CODE\
	"# Operator '.'" _OPSEP\
	"LABEL !*op_dot\n"\
	"PUSHFRAME\n"\
	"\n"\
	"DEFVAR LF@op1\n"\
	"DEFVAR LF@op2\n"\
	"DEFVAR LF@op1type\n"\
	"DEFVAR LF@op2type\n"\
	"DEFVAR LF@res\n"\
	"\n"\
	"POPS LF@op2\n"\
	"POPS LF@op1\n"\
	"\n"\
	"TYPE LF@op1type LF@op1\n"\
	"TYPE LF@op2type LF@op2\n"\
	"\n"\
	"JUMPIFEQ !*op_dot_op1_nil LF@op1type string@nil\n"\
	"LABEL !*op_dot_check_op2_nil\n"\
	"JUMPIFEQ !*op_dot_op2_nil LF@op2type string@nil\n"\
	"\n"\
	"LABEL !*op_dot_check_op1_string\n"\
	"TYPE LF@op1type LF@op1\n"\
	"TYPE LF@op2type LF@op2\n"\
	"\n"\
	"JUMPIFNEQ !*sem_error_seven LF@op1type string@string\n"\
	"JUMPIFNEQ !*sem_error_seven LF@op2type string@string\n"\
	"JUMP !*op_dot_do\n"\
	"\n"\
	"LABEL !*op_dot_op1_nil\n"\
	"	MOVE LF@op1 string@\n"\
	"	JUMP !*op_dot_check_op2_nil\n"\
	"\n"\
	"LABEL !*op_dot_op2_nil\n"\
	"	MOVE LF@op2 string@\n"\
	"	JUMP !*op_dot_check_op1_string\n"\
	"\n"\
	"LABEL !*op_dot_do\n"\
	"	PUSHS LF@op1\n"\
	"	PUSHS LF@op2\n"\
	"	CONCAT LF@res LF@op1 LF@op2\n"\
	"	PUSHS LF@res\n"\
	"\n"\
	"LABEL !*op_dot_end\n"\
	"POPFRAME\n"\
	"RETURN\n" _BFNEND

#define OP_EQ_NEQ_CODE\
	"# Operators '===', '!=='" _OPSEP\
	"LABEL !*op_eq_neq\n"\
	"PUSHFRAME\n"\
	"\n"\
	"DEFVAR LF@op1\n"\
	"DEFVAR LF@op2\n"\
	"DEFVAR LF@op1type\n"\
	"DEFVAR LF@op2type\n"\
	"DEFVAR LF@rule\n"\
	"\n"\
	"POPS LF@rule\n"\
	"POPS LF@op2\n"\
	"POPS LF@op1\n"\
	"\n"\
	"TYPE LF@op1type LF@op1\n"\
	"TYPE LF@op2type LF@op2\n"\
	"\n"\
	"PUSHS LF@op1\n"\
	"PUSHS LF@op2\n"\
	"\n"\
	"JUMPIFEQ !*op_eq_neq_equal LF@rule string@equal\n"\
	"JUMPIFEQ !*op_eq_neq_noteq LF@rule string@noteq\n"\
	"\n"\
	"JUMP !*sem_error_eight\n"\
	"\n"\
	"LABEL !*op_eq_neq_equal\n"\
		"JUMPIFNEQ !*op_eq_neq_false LF@op1type LF@op2type\n"\
		"EQS\n"\
		"JUMP !*op_eq_neq_end\n"\
	"\n"\
	"LABEL !*op_eq_neq_noteq\n"\
		"JUMPIFNEQ !*op_eq_neq_true LF@op1type LF@op2type\n"\
		"EQS\n"\
		"NOTS\n"\
		"JUMP !*op_eq_neq_end\n"\
	"\n"\
	"LABEL !*op_eq_neq_false\n"\
	"	PUSHS bool@false\n"\
	"	JUMP !*op_eq_neq_end\n"\
	"\n"\
	"LABEL !*op_eq_neq_true\n"\
	"	PUSHS bool@true\n"\
	"\n"\
	"LABEL !*op_eq_neq_end\n"\
	"POPFRAME\n"\
	"RETURN\n" _BFNEND

#define OP_COMPARE_CODE\
	"# Operator '>', '<', '>=', '<='" _OPSEP\
	"LABEL !*op_rel\n"\
	"PUSHFRAME\n"\
	"\n"\
	"DEFVAR LF@op1\n"\
	"DEFVAR LF@op2\n"\
	"DEFVAR LF@op1type\n"\
	"DEFVAR LF@op2type\n"\
	"DEFVAR LF@rule\n"\
	"\n"\
	"POPS LF@rule\n"\
	"POPS LF@op2\n"\
	"POPS LF@op1\n"\
	"\n"\
	"TYPE LF@op1type LF@op1\n"\
	"TYPE LF@op2type LF@op2\n"\
	"\n"\
	"JUMPIFEQ !*op_rel_op1_nil LF@op1type string@nil\n"\
	"JUMPIFEQ !*op_rel_op2_nil LF@op2type string@nil\n"\
	"\n"\
	"JUMPIFEQ !*op_rel_op1_int LF@op1type string@int\n"\
	"JUMPIFEQ !*op_rel_op2_int LF@op2type string@int\n"\
	"\n"\
	"JUMPIFEQ !*op_rel_op1_float LF@op1type string@float\n"\
	"\n"\
	"JUMPIFEQ !*op_rel_op1_str LF@op1type string@string\n"\
	"\n"\
	"LABEL !*op_rel_op1_float\n"\
	"    JUMPIFNEQ !*sem_error_seven LF@op2type string@float\n"\
	"    JUMP !*op_rel_do\n"\
	"\n"\
	"LABEL !*op_rel_op1_str\n"\
	"    JUMPIFNEQ !*sem_error_seven LF@op2type string@string\n"\
	"    JUMP !*op_rel_do\n"\
	"\n"\
	"LABEL !*op_rel_op1_int\n"\
	"    JUMPIFEQ !*op_rel_op1_int_op2_float LF@op2type string@float\n"\
	"    JUMPIFEQ !*op_rel_do LF@op2type string@int\n"\
	"    JUMP !*sem_error_seven\n"\
	"\n"\
	"LABEL !*op_rel_op1_int_op2_float\n"\
	"    INT2FLOAT LF@op1 LF@op1\n"\
	"    JUMP !*op_rel_do\n"\
	"\n"\
	"LABEL !*op_rel_op2_int\n"\
	"    JUMPIFNEQ !*sem_error_seven LF@op1type string@float\n"\
	"    INT2FLOAT LF@op2 LF@op2\n"\
	"    JUMP !*op_rel_do\n"\
	"\n"\
	"LABEL !*op_rel_op1_nil\n"\
	"    JUMPIFEQ !*op_rel_op1_nil_op2_nil LF@op2type string@nil\n"\
	"    JUMPIFEQ !*op_rel_op1_nil_op2_str LF@op2type string@string\n"\
	"    JUMPIFEQ !*op_rel_op1_nil_op2_int LF@op2type string@int\n"\
	"    JUMPIFEQ !*op_rel_op1_nil_op2_float LF@op2type string@float\n"\
	"    JUMP !*sem_error_seven\n"\
	"\n"\
	"    LABEL !*op_rel_op1_nil_op2_nil\n"\
	"        MOVE LF@op1 bool@false\n"\
	"        MOVE LF@op2 bool@false\n"\
	"        JUMP !*op_rel_do\n"\
	"\n"\
	"    LABEL !*op_rel_op1_nil_op2_str\n"\
	"        MOVE LF@op1 string@\n"\
	"        JUMP !*op_rel_do\n"\
	"\n"\
	"    LABEL !*op_rel_op1_nil_op2_int\n"\
	"        MOVE LF@op1 bool@false\n"\
	"\n"\
	"        JUMPIFEQ !*op_rel_op1_nil_op2_int_zero LF@op2 int@0\n"\
	"        MOVE LF@op2 bool@true\n"\
	"        JUMP !*op_rel_do\n"\
	"\n"\
	"        LABEL !*op_rel_op1_nil_op2_int_zero\n"\
	"            MOVE LF@op2 bool@false\n"\
	"            JUMP !*op_rel_do\n"\
	"    \n"\
	"    LABEL !*op_rel_op1_nil_op2_float\n"\
	"        MOVE LF@op1 bool@false\n"\
	"\n"\
	"        JUMPIFEQ !*op_rel_op1_nil_op2_float_zero LF@op2 float@0x0.0p+0\n"\
	"        MOVE LF@op2 bool@true\n"\
	"        JUMP !*op_rel_do\n"\
	"\n"\
	"        LABEL !*op_rel_op1_nil_op2_float_zero\n"\
	"            MOVE LF@op2 bool@false\n"\
	"            JUMP !*op_rel_do\n"\
	"\n"\
	"LABEL !*op_rel_op2_nil\n"\
	"    JUMPIFEQ !*op_rel_op2_nil_op1_str LF@op1type string@string\n"\
	"    JUMPIFEQ !*op_rel_op2_nil_op1_int LF@op1type string@int\n"\
	"    JUMPIFEQ !*op_rel_op2_nil_op1_float LF@op1type string@float\n"\
	"    JUMP !*sem_error_seven\n"\
	"\n"\
	"    LABEL !*op_rel_op2_nil_op1_str\n"\
	"        MOVE LF@op2 string@\n"\
	"        JUMP !*op_rel_do\n"\
	"\n"\
	"    LABEL !*op_rel_op2_nil_op1_int\n"\
	"        MOVE LF@op2 bool@false\n"\
	"\n"\
	"        JUMPIFEQ !*op_rel_op2_nil_op1_int_zero LF@op1 int@0\n"\
	"        MOVE LF@op1 bool@true\n"\
	"        JUMP !*op_rel_do\n"\
	"\n"\
	"        LABEL !*op_rel_op2_nil_op1_int_zero\n"\
	"            MOVE LF@op1 bool@false\n"\
	"            JUMP !*op_rel_do\n"\
	"    \n"\
	"    LABEL !*op_rel_op2_nil_op1_float\n"\
	"        MOVE LF@op2 bool@false\n"\
	"\n"\
	"        JUMPIFEQ !*op_rel_op2_nil_op1_float_zero LF@op1 float@0x0.0p+0\n"\
	"        MOVE LF@op1 bool@true\n"\
	"        JUMP !*op_rel_do\n"\
	"\n"\
	"        LABEL !*op_rel_op2_nil_op1_float_zero\n"\
	"            MOVE LF@op1 bool@false\n"\
	"            JUMP !*op_rel_do\n"\
	"\n"\
	"LABEL !*op_rel_do\n"\
	"PUSHS LF@op1\n"\
	"PUSHS LF@op2\n"\
	"\n"\
	"JUMPIFEQ !*op_rel_gt LF@rule string@gt\n"\
	"JUMPIFEQ !*op_rel_gt LF@rule string@geq\n"\
	"JUMPIFEQ !*op_rel_lt LF@rule string@lt\n"\
	"JUMPIFEQ !*op_rel_lt LF@rule string@leq\n"\
	"JUMP !*sem_error_eight\n"\
	"\n"\
	"LABEL !*op_rel_gt\n"\
	"    GTS\n"\
	"    JUMPIFEQ !*op_rel_leq_geq LF@rule string@geq\n"\
	"    JUMP !*op_rel_end\n"\
	"\n"\
	"LABEL !*op_rel_lt\n"\
	"    LTS\n"\
	"    JUMPIFEQ !*op_rel_leq_geq LF@rule string@leq\n"\
	"    JUMP !*op_rel_end\n"\
	"\n"\
	"LABEL !*op_rel_leq_geq\n"\
	"    PUSHS LF@op1\n"\
	"    PUSHS LF@op2\n"\
	"    EQS\n"\
	"    ORS\n"\
	"\n"\
	"LABEL !*op_rel_end\n"\
	"POPFRAME\n"\
	"RETURN\n" _BFNEND

#define ERROR_LABELS_CODE\
	"# Runtime error labels" _OPSEP\
	"LABEL !*sem_error_three\n"\
	"EXIT int@3\n"\
	"LABEL !*sem_error_four\n"\
	"EXIT int@4\n"\
	"LABEL !*sem_error_five\n"\
	"EXIT int@5\n"\
	"LABEL !*sem_error_six\n"\
	"EXIT int@6\n"\
	"LABEL !*sem_error_seven\n"\
	"EXIT int@7\n"\
	"LABEL !*sem_error_eight\n"\
	"EXIT int@8\n" _BFNEND

#define EXPR_RES_BOOL_VALIDATE_CODE\
	"# Expression result bool convert" _OPSEP\
	"LABEL !*expr_res_bool_convert\n"\
	"PUSHFRAME\n"\
	"\n"\
	"DEFVAR LF@expr_type\n"\
	"TYPE LF@expr_type GF@EXPR_VAL\n"\
	"\n"\
	"JUMPIFEQ !*erbc_end LF@expr_type string@bool\n"\
	"JUMPIFEQ !*erbc_type_string LF@expr_type string@string\n"\
	"JUMPIFEQ !*erbc_type_int LF@expr_type string@int\n"\
	"JUMPIFEQ !*erbc_type_float LF@expr_type string@float\n"\
	"JUMPIFEQ !*erbc_false LF@expr_type string@nil\n"\
	"JUMP !*sem_error_seven\n"\
	"\n"\
	"LABEL !*erbc_type_string\n"\
	"	JUMPIFEQ !*erbc_false GF@EXPR_VAL string@\n"\
	"	JUMPIFEQ !*erbc_false GF@EXPR_VAL string@0\n"\
	"	JUMP !*erbc_true\n"\
	"\n"\
	"LABEL !*erbc_type_int\n"\
	"	JUMPIFEQ !*erbc_false GF@EXPR_VAL int@0\n"\
	"	JUMP !*erbc_true\n"\
	"\n"\
	"LABEL !*erbc_type_float\n"\
	"	JUMPIFEQ !*erbc_false GF@EXPR_VAL float@0x0.0p+0\n"\
	"	JUMP !*erbc_true\n"\
	"\n"\
	"LABEL !*erbc_false\n"\
	"	MOVE GF@EXPR_VAL bool@false\n"\
	"   JUMP !*erbc_end\n"\
	"\n"\
	"LABEL !*erbc_true\n"\
	"	MOVE GF@EXPR_VAL bool@true\n"\
	"\n"\
	"LABEL !*erbc_end\n"\
	"POPFRAME\n"\
	"RETURN\n" _BFNEND

#define RETURN_SEMANTIC_CHECK\
	"# Return semantic check" _OPSEP\
	"LABEL !*return_semantic_check\n"\
	"PUSHFRAME\n"\
	"\n"\
	"DEFVAR LF@func_type\n"\
	"POPS LF@func_type\n"\
	"DEFVAR LF@func_qmark\n"\
	"POPS LF@func_qmark\n"\
	"TYPE GF@EXPR_TYPE GF@EXPR_VAL\n"\
	"\n"\
	"JUMPIFEQ !*rs_ok GF@EXPR_TYPE LF@func_type\n"\
	"JUMPIFNEQ !*sem_error_four LF@func_qmark bool@true\n"\
	"JUMPIFNEQ !*sem_error_four GF@EXPR_TYPE string@nil\n"\
	"\n"\
	"LABEL !*rs_ok\n"\
	"POPFRAME\n"\
	"RETURN\n" _BFNEND

#endif // __INTERNALS_H__