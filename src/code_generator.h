#ifndef __CODE_GENERATOR__
#define __CODE_GENERATOR__

#include <stdbool.h>
#include <stdio.h>
#include <ctype.h>

#include "symtable.h"
#include "string_t.h"
#include "scanner.h"

#define ADD_CODE(text)                              \
    if (!str_concat(&code, (text))) return false

#define ADD_CODE_N(text)                            \
    if (!str_concat(&code, (text"\n"))) return false

#define ADD_NUM(number)                 \
    do {                                \
        char str[MAX_DIGITS];           \
        sprintf(str, "%ld", (number));  \
        ADD_CODE(str);                  \
    } while (0)

#define MAX_DIGITS 50

/**
 * Generation of header code.
 * @return
 */
bool generate_header();

/**
 * Generation of code for built-in functions.
 * @return true if success.
 */
bool generate_built_in_funcs();

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
bool generate_main_start();

/**
 * Main end generation.
 * @return true if success.
 */
bool generate_main_end();

/**
 * Generation of function beginning.
 * @param name
 * @return true if success.
 */
bool generate_function_start(char* name);

/**
 * Generation of function end.
 * @param name
 * @return true if success.
 */
bool generate_function_end(char* name);

/**
 * Generation of default variable value.
 * @param type
 * @return true if success.
 */
bool generate_def_val(Data_type type);

/**
 * Generation of @res value.
 * @param type
 * @return true if success.
 */
bool generate_function_res(Data_type type);

/**
 * Generation of defvar.
 * @param var
 * @return true if success.
 */
bool generate_defvar(char* var);

/**
 * Giving a variable default value.
 * @param type
 * @param var
 * @return true if success.
 */
bool generate_var_def(Data_type type, char* var);

/**
 * Generation of function calling.
 * @param name
 * @return true if success.
 */
bool generate_function_call(char* name);

/**
 * Generation of value from token.
 * @param token
 * @return true if success.
 */
bool generate_value_from_token(Token token);

/**
 * Generation of part before passing parameters into function.
 * @return true if success.
 */
bool generate_function_before_pass_params();

/**
 * Generation of passing parameters into function.
 * @param token
 * @param index
 * @return true if success.
 */
bool generate_function_pass_param(Token token, int64_t index);

/**
 * Generation of returning from function.
 * @param name
 * @return true if success.
 */
bool generate_function_return(char* name);

/**
 * Generation of pushing data to data stack.
 * @param token
 * @return true if success.
 */
bool generate_push(Token token);

#endif //__CODE_GENERATOR__
