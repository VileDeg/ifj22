#ifndef __CODE_GENERATOR__
#define __CODE_GENERATOR__

#include <stdbool.h>
#include <stdio.h>

#include "string_t.h"

#define ADD_CODE(text)                              \
    if (!str_concat(&code, (text))) return false

#define ADD_CODE_N(text)                            \
    if (!str_concat(&code, (text"\n"))) return false

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
 * @return
 */
bool generate_main_end();

#endif //__CODE_GENERATOR__
