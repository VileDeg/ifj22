#ifndef __CODE_GENERATOR__
#define __CODE_GENERATOR__

#include <stdbool.h>
#include <stdio.h>
#include <ctype.h>

#include "symtable.h"
#include "string_t.h"
#include "scanner.h"
#include "precedence_t.h"


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

/**
 * Starting generation of code.
 * @return true if success.
 */
bool code_generator_start();

/**
 * Finish code generation.
 */
void code_generator_finish();

/**
 * Extract code into file.
 * @param file
 */
void code_generator_flush(FILE* file);

/**
 * Main beginning generation.
 * @return true if success.
 */
bool emit_body_start();

/**
 * Main end generation.
 * @return true if success.
 */
bool emit_body_end();

/**
 * Generation of function beginning.
 * @param name
 * @return true if success.
 */
bool emit_function_start(char* name);

/**
 * Generation of function end.
 * @param name
 * @return true if success.
 */
bool emit_function_end(char* name);

/**
 * Generation of default variable value.
 * @param type
 * @return true if success.
 */
bool emit_def_val(Data_type type);

/**
 * Generation of @res value.
 * @param type
 * @return true if success.
 */
bool emit_function_res(Data_type type);

/**
 * Generation of defvar.
 * @param var
 * @return true if success.
 */
bool emit_defvar(char* var);

/**
 * Giving a variable default value.
 * @param type
 * @param var
 * @return true if success.
 */
bool emit_var_def(Data_type type, char* var);

/**
 * Generation of function calling.
 * @param name
 * @return true if success.
 */
bool emit_function_call(char* name);

/**
 * Generation of function result assignment.
 * @param var
 * @param var_type
 * @param res_type
 * @return true if success.
 */
bool emit_function_res_assign(char* var, Data_type var_type, Data_type res_type);

/**
 * Generation of local variables from parameters.
 * @param name
 * @param index
 * @return true if success.
 */
bool emit_function_param_declare(char* name, int64_t index);

/**
 * Generation of value from token.
 * @param token
 * @return true if success.
 */
bool emit_value_from_token(Token token);

/**
 * Generation of part before passing parameters into function.
 * @return true if success.
 */
bool emit_function_before_pass_params();

/**
 * Generation of parameter type conversion.
 * @param from
 * @param to
 * @param index
 * @return true if success.
 */
bool emit_function_convert_passed_param(Data_type from, Data_type to, int64_t index);

/**
 * Generation of passing parameters into function.
 * @param token
 * @param index
 * @return true if success.
 */
bool emit_function_pass_param(Token token, int64_t index);

/**
 * Generation of returning from function.
 * @param name
 * @return true if success.
 */
bool emit_function_return(char* name);

/**
 * Generation of input.
 * @param name
 * @param type
 * @return true if success.
 */
bool emit_input(char* var, Data_type type);

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
bool emit_push(Token token);

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
 * @param res_type
 * @param var_type
 * @param frame
 * @return true if success.
 */
bool emit_stack_pop_res(char* var, Data_type res_type, Data_type var_type, char* frame);

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
bool emit_label(char* name, int64_t deep, int64_t index);

/**
 * Generation of if start.
 * @param name
 * @param deep
 * @param index
 * @return true if success.
 */
bool emit_if_jump(char* name, int64_t deep, int64_t index);

/**
 * Generation of else start.
 * @param name
 * @param deep
 * @param index
 * @return true if success.
 */
bool emit_else_jump(char* name, int64_t deep, int64_t index);

/**
 * Generation of if end.
 * @param name
 * @param deep
 * @param index
 * @return true if success.
 */
bool emit_if_end(char* name, int64_t deep, int64_t index);

/**
 * Generation of while head.
 * @param name
 * @param deep
 * @param index
 * @return true if success.
 */
bool emit_while_head(char* name, int64_t deep, int64_t index);

/**
 * Generation of while start.
 * @param name
 * @param deep
 * @param index
 * @return true if success.
 */
bool emit_while_start(char* name, int64_t deep, int64_t index);

/**
 * Generation of while end.
 * @param name
 * @param deep
 * @param index
 * @return true if success.
 */
bool emit_while_end(char* name, int64_t deep, int64_t index);

#endif //__CODE_GENERATOR__
