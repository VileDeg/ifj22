#ifndef __CODEGEN_H__
#define __CODEGEN_H__

#include <stdbool.h>
#include <stdio.h>
#include <ctype.h>

#include "symtable.h"
#include "string_t.h"
#include "scanner.h"
#include "expression.h"
#include "macros.h"

#ifdef VILE_DEBUG
	#define CODEGEN(_funcptr, ...) do {\
		if (pd->mode == MODE_MAIN_PASS)\
		{\
			if (!_funcptr(__VA_ARGS__)) INTERNAL_ERROR_RET();\
		}\
	}while(0)
#else
	#define CODEGEN(_funcptr, ...) if (!_funcptr(__VA_ARGS__)) return ERROR_INTERNAL
#endif //VILE_DEBUG


void set_codegen_out(FILE* out);

/**
 * Generation of header code.
 * @return
 */
bool emit_header();

/**
 * Generation of code for built-in functions.
 * @return true if success.
 */
bool emit_built_in_funcs();

bool emit_program_body_open();

bool emit_program_body_close();

/**
 * Starting generation of code.
 * @return true if success.
 */
bool code_generator_init();

/**
 * Finish code generation.
 */
void code_generator_terminate();

bool emit_expression_bool_convert();
/**
 * Extract code into file.
 * @param file
 */
void code_generator_flush();

bool emit_clear_stack();

bool emit_function_type(DataType type, bool questionmark);

bool emit_call_return_sem_check();

bool emit_check_var_defined(const char* id, bool local);

/**
 * Generation of function beginning.
 * @param name
 * @return true if success.
 */
bool emit_function_open(const char* name);

/**
 * Generation of function end.
 * @param name
 * @return true if success.
 */
bool emit_function_close(const char* name);

// /**
//  * Generation of default variable value.
//  * @param type
//  * @return true if success.
//  */
bool emit_default_value(DataType type);

/**
 * Generation of @res value.
 * @param type
 * @return true if success.
 */
bool emit_function_result_declaration(DataType type);

/**
 * Generation of defvar.
 * @param var
 * @param in_local_scope
 * @return true if success.
 */
bool emit_define_var(const char* var, bool in_local_scope);

// /**
//  * Giving a variable default value.
//  * @param type
//  * @param var
//  * @return true if success.
//  */
bool emit_var_default_value(DataType type, const char* var);

/**
 * Generation of function calling.
 * @param name
 * @return true if success.
 */
bool emit_function_call(const char* name);


//bool emit_function_result_assign(const char* var, DataType var_type, DataType res_type);
bool emit_function_result_assign(const char* var_name, bool local_frame);

/**
 * Generation of local variables from parameters.
 * @param name
 * @param index
 * @return true if success.
 */
bool emit_function_param_declare(const char* name, int64_t index);

/**
 * Generation of value from token.
 * @param token
 * @return true if success.
 */
bool emit_value_from_token(Token token, bool local_frame);

/**
 * Generation of part before passing parameters into function.
 * @return true if success.
 */
bool emit_function_before_params();

// /**
//  * Generation of parameter type conversion.
//  * @param from
//  * @param to
//  * @param index
//  * @return true if success.
//  */
// bool emit_function_convert_passed_param(DataType from, DataType to, int64_t index);


// inline bool emit_expr_res_bool_check()
// {
// 	EMIT_NL(EXPR_RES_BOOL_CHECK);
// }

/**
 * Generation of passing parameters into function.
 * @param token
 * @param index
 * @return true if success.
 */
bool emit_function_pass_param(Token token, int64_t index, bool local_frame);

/**
 * Generation of passing parameters into function by stack.
 * @param token
 * @return true if success.
 */
bool emit_function_pass_param_push(Token token, bool local_frame);

bool emit_function_pass_param_count(int64_t count);

/**
 * Generation of returning from function.
 * @param name
 * @return true if success.
 */
bool emit_function_return_statement(const char* name, bool is_void);

/**
 * Generation of pushing data to data stack.
 * @param token
 * @return true if success.
 */
bool emit_push_token(Token token, bool local_frame);

/**
 * Generation of code for stack operation.
 * @param rule
 * @return true if success.
 */
bool emit_operator_call(Rule_type rule);

/**
 * Generation of popping result from stack.
 * @param var
 * @param frame
 * @return true if success.
 */
bool emit_pop_expr_result(const char* var, const char* frame);

/**
 * Generation of converting top element
 * on stack from int to float.
 * @return true if success.
 */
bool emit_stack_top_int2float();

/**
 * Generation of converting top element
 * on stack from float to int.
 * @return true if success.
 */
bool emit_stack_top_float2int();

/**
 * Generation of converting second
 * element on stack from int to float.
 * @return true if success.
 */
bool emit_stack_sec_int2float();

/**
 * Generation of converting second
 * element on stack from float to int.
 * @return true if success.
 */
bool emit_stack_sec_float2int();

/**
 * Generation of label.
 * @param name
 * @param deep
 * @param index
 * @return true if success.
 */
bool emit_condition_label(const char* name, int64_t deep, int64_t index);

bool emit_if_header();

/**
 * Generation of if start.
 * @param name
 * @param deep
 * @param index
 * @return true if success.
 */
bool emit_if_open(const char* name, int64_t deep, int64_t index);

/**
 * Generation of else start.
 * @param name
 * @param deep
 * @param index
 * @return true if success.
 */
bool emit_else(const char* name, int64_t deep, int64_t index);

/**
 * Generation of if end.
 * @param name
 * @param deep
 * @param index
 * @return true if success.
 */
bool emit_if_close(const char* name, int64_t deep, int64_t index);

/**
 * Generation of while head.
 * @param name
 * @param deep
 * @param index
 * @return true if success.
 */
bool emit_while_header(const char* name, int64_t deep, int64_t index);

/**
 * Generation of while start.
 * @param name
 * @param deep
 * @param index
 * @return true if success.
 */
bool emit_while_open(const char* name, int64_t deep, int64_t index);

/**
 * Generation of while end.
 * @param name
 * @param deep
 * @param index
 * @return true if success.
 */
bool emit_while_close(const char* name, int64_t deep, int64_t index);

#endif //__CODEGEN_H__
