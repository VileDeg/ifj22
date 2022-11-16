#ifndef __CODE_GENERATOR__
#define __CODE_GENERATOR__

#include <stdbool.h>
#include <stdio.h>
#include <ctype.h>

#include "symtable.h"
#include "string_t.h"
#include "scanner.h"
#include "precedence_t.h"

extern FILE* g_CodegenOut;
#ifdef IFJ22_DEBUG
	#define CODEGEN(_funcptr, ...) do { if (!_funcptr(__VA_ARGS__)) return ERROR_INTERNAL;\
		IFJ22_ASSERT(g_CodegenOut, "Code generator output file not found");\
		code_generator_flush(g_CodegenOut); }while(0)
#else
	#define CODEGEN(_funcptr, ...) if (!_funcptr(__VA_ARGS__)) return ERROR_INTERNAL
#endif //IFJ22_DEBUG
 
extern str_t g_Code;



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

/**
 * Starting generation of code.
 * @return true if success.
 */
bool code_generator_init();

/**
 * Finish code generation.
 */
void code_generator_terminate();

/**
 * Extract code into file.
 * @param file
 */
void code_generator_flush(FILE* file);

bool emit_push_bool_literal(bool value);

bool emit_pop();

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
// bool emit_def_val(DataType type);

/**
 * Generation of @res value.
 * @param type
 * @return true if success.
 */
bool emit_function_res(DataType type);

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
// bool emit_var_def(DataType type, const char* var);

/**
 * Generation of function calling.
 * @param name
 * @return true if success.
 */
bool emit_function_call(const char* name);


//bool emit_function_res_assign(const char* var, DataType var_type, DataType res_type);
bool emit_function_res_assign(const char* var_name, bool local_frame);

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
bool emit_function_before_pass_params();

// /**
//  * Generation of parameter type conversion.
//  * @param from
//  * @param to
//  * @param index
//  * @return true if success.
//  */
// bool emit_function_convert_passed_param(DataType from, DataType to, int64_t index);

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
bool emit_function_return(const char* name);

// /**
//  * Generation of input.
//  * @param name
//  * @param type
//  * @return true if success.
//  */
// bool emit_input(const char* var, DataType type);

/**
 * Generation of writing expression result.
 * @return true if success.
 */
bool emit_exp_res();

/**
 * Generation of pushing data to data stack.
 * @param token
 * @return true if success.
 */
bool emit_push(Token token, bool local_frame);

/**
 * Generation of code for stack operation.
 * @param rule
 * @return true if success.
 */
bool emit_stack_operation(Rule_type rule);

/**
 * Generation of stack elements concatenation.
 * @return true if success.
 */
bool emit_stack_concat();

/**
 * Generation of popping result from stack.
 * @param var
 * @param frame
 * @return true if success.
 */
bool emit_stack_pop_res(const char* var, const char* frame);

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
bool emit_label(const char* name, int64_t deep, int64_t index);

bool emit_if_head();

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
bool emit_while_head(const char* name, int64_t deep, int64_t index);

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

#endif //__CODE_GENERATOR__
