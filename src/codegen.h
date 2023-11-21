/**
 * Project: Implementation of imperative language IFJ22 compiler.
 * 
 * @author Aleksandr Shevchenko (xshevc01@stud.fit.vutbr.cz)
 * @author Vadim Goncearenco   (xgonce00@stud.fit.vutbr.cz)
 */

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

#ifdef VILE_DEBUG /* Helper macro for calling code generator functions */
	#define CODEGEN(_funcptr, ...) do {\
		if (pd->mode == MODE_MAIN_PASS)\
		{\
			if (!_funcptr(__VA_ARGS__)) INTERNAL_ERROR_RET();\
		}\
	}while(0)
#else
	#define CODEGEN(_funcptr, ...) if (!_funcptr(__VA_ARGS__)) return ERROR_INTERNAL
#endif //VILE_DEBUG

/**
 * Code generator output(for debug).
 * @param out
 */
void set_codegen_out(FILE* out);

/**
 * Starting generation of code.
 * @return True if success.
 */
bool codegen_init();

/**
 * Finish code generation.
 */
void codegen_terminate();

/**
 * Extract code into file.
 */
void codegen_flush();

/**
 * Generation of header code.
 * @return True if success.
 */
bool emit_header();

/**
 * Generation of code for built-in functions.
 * @return True if success.
 */
bool emit_built_in_funcs();

/**
 * Generation of code for program body start.
 * @return True if success.
 */
bool emit_program_body_open();

/**
 * Generation of code for program body end.
 * @return True if success.
 */
bool emit_program_body_close();

/**
 * Generation of code for call of internal function
 * for implicit conversion of expression to boolean
 * @return True if success.
 */
bool emit_expression_bool_convert();

/**
 * Generation of code for instruction that clears stack
 * @return True if success.
 */
bool emit_clear_stack();

/**
 * Generation of code for instruction that clears stack
 * @param type type of function
 * @param questionmark function can return null
 * @return True if success.
 */
bool emit_function_type(DataType type, bool questionmark);

/**
 * Generation of code for call of internal function
 * that checks semantic of expression returned from function
 * @return True if success.
 */
bool emit_call_return_sem_check();

/**
 * Generation of code that will throw error if variable was not defined
 * @param id variable identifier
 * @param local variable is local
 * @return True if success.
 */
bool emit_check_var_defined(const char* id, bool local);

/**
 * Generation of function beginning.
 * @param name function name
 * @return True if success.
 */
bool emit_function_open(const char* name);

/**
 * Generation of function end.
 * @param name function name
 * @return True if success.
 */
bool emit_function_close(const char* name);

/**
 * Generation of @res value.
 * @param type function type
 * @return True if success.
 */
bool emit_function_result_declaration(DataType type);

/**
 * Generation of DEFVAR instruction for variable.
 * @param var variable name
 * @param in_local_scope definition in local scope
 * @return True if success.
 */
bool emit_define_var(const char* var, bool in_local_scope);

/**
 * Generation of function call.
 * @param name function name
 * @return True if success.
 */
bool emit_function_call(const char* name);

/**
 * Generation of function code to assign function result to variable.
 * @param var_name variable name
 * @param local_frame assignment in local scope
 * @return True if success.
 */
bool emit_function_result_assign(const char* var_name, bool local_frame);

/**
 * Generation of local variables from parameters.
 * @param name parameter name
 * @param index parameter index
 * @return True if success.
 */
bool emit_function_param_declare(const char* name, int64_t index);

/**
 * Generation of value from token.
 * @param token token
 * @return True if success.
 */
bool emit_value_from_token(Token token, bool local_frame);

/**
 * Generation of part before passing parameters into function.
 * @return True if success.
 */
bool emit_function_before_params();

/**
 * Generation of passing parameters into function.
 * @param token token
 * @param index parameter index
 * @return True if success.
 */
bool emit_function_pass_param(Token token, int64_t index, bool local_frame);

/**
 * Generation of passing parameters to function by stack.
 * @param token token
 * @return True if success.
 */
bool emit_function_pass_param_push(Token token, bool local_frame);

/**
 * Generation of passing parameter count to function by stack.
 * @param count parameter count
 * @return True if success.
 */
bool emit_function_pass_param_count(int64_t count);

/**
 * Generation of returning from function.
 * @param name
 * @return True if success.
 */
bool emit_function_return_statement(const char* name, bool is_void);

/**
 * Generation of pushing data to data stack.
 * @param token
 * @return True if success.
 */
bool emit_push_token(Token token, bool local_frame);

/**
 * Generation of code for stack operation.
 * @param rule expression rule
 * @return True if success.
 */
bool emit_operator_call(Rule_type rule);

/**
 * Generation of popping result from stack.
 * @param var variable name
 * @param frame local/global frame
 * @return True if success.
 */
bool emit_pop_expr_result(const char* var, const char* frame);

/**
 * Generation of label for if/while.
 * @param name function name in which if or while is found
 * @param deep number of scopes if/while is found in
 * @param index index of if/while labels to distinct them
 * @return True if success.
 */
bool emit_condition_label(const char* name, int64_t deep, int64_t index);

/**
 * Generation of code for source code header.
 * @return True if success.
 */
bool emit_if_header();

/**
 * Generation of if start.
 * @param name function name in which if or while is found
 * @param deep number of scopes if/while is found in
 * @param index index of if/while labels to distinct them
 * @return True if success.
 */
bool emit_if_open(const char* name, int64_t deep, int64_t index);

/**
 * Generation of else start.
 * @param name function name in which if or while is found
 * @param deep number of scopes if/while is found in
 * @param index index of if/while labels to distinct them
 * @return True if success.
 */
bool emit_else(const char* name, int64_t deep, int64_t index);

/**
 * Generation of if end.
 * @param name function name in which if or while is found
 * @param deep number of scopes if/while is found in
 * @param index index of if/while labels to distinct them
 * @return True if success.
 */
bool emit_if_close(const char* name, int64_t deep, int64_t index);

/**
 * Generation of while head.
 * @param name function name in which if or while is found
 * @param deep number of scopes if/while is found in
 * @param index index of if/while labels to distinct them
 * @return True if success.
 */
bool emit_while_header(const char* name, int64_t deep, int64_t index);

/**
 * Generation of while start.
 * @param name function name in which if or while is found
 * @param deep number of scopes if/while is found in
 * @param index index of if/while labels to distinct them
 * @return True if success.
 */
bool emit_while_open(const char* name, int64_t deep, int64_t index);

/**
 * Generation of while end.
 * @param name function name in which if or while is found
 * @param deep number of scopes if/while is found in
 * @param index index of if/while labels to distinct them
 * @return True if success.
 */
bool emit_while_close(const char* name, int64_t deep, int64_t index);

#endif //__CODEGEN_H__
