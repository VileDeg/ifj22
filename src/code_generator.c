#include "code_generator.h"

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
* GENERATOR OF BUILT-IN FUNCTIONS
*/

/// function reads() : ?string
#define FUNCTION_READS              \
        "# Function reads\n"        \
        "LABEL @reads\n"            \
        "PUSHFRAME\n"               \
        "DEFVAR LF@res\n"           \
        "READ LF@res string\n"      \
        "POPFRAME\n"                \
        "RETURN\n"

/// function readi() : ?int
#define FUCNTION_READI          \
        "# Function readi\n"    \
        "LABEL @readi\n"        \
        "PUSHFRAME\n"           \
        "DEFVAR LF@res\n"       \
        "READ LF@res int\n"     \
        "POPFRAME\n"            \
        "RETURN\n"

/// function readf() : ?float
#define FUNCTION_READF              \
        "# Function readf\n"        \
        "LABEL @readf\n"            \
        "PUSHFRAME\n"               \
        "DEFVAR LF@res\n"           \
        "READ LF@res float\n"       \
        "POPFRAME\n"                \
        "RETURN\n"

/// function strlen(string $𝑠) : int
#define FUNCTION_STRLEN                 \
        "# Function strlen\n"           \
        "LABEL @strlen\n"               \
        "PUSHFRAME\n"                   \
        "DEFVAR LF@res\n"               \
        "STRLEN LF@res LF@-0\n"         \
        "POPFRAME\n"                    \
        "RETURN\n"

/// function substring(string $𝑠, int $𝑖, int $𝑗) : ?string
#define FUNCTION_SUBSTRING                              \
        "# Function substring\n"                        \
        "LABEL @substring\n"                            \
        "PUSHFRAME\n"                                   \
        "DEFVAR LF@res\n"                               \
        "MOVE LF@res string@\n"                         \
        "DEFVAR LF@length\n"                            \
        "CREATEFRAME\n"                                 \
        "DEFVAR TF@-0\n"                                \
        "MOVE TF@-0 LF@-0\n"                            \
        "CALL @strlen\n"                                \
        "MOVE LF@length TF@res\n"                       \
        "DEFVAR LF@cond\n"                              \
        "LT LF@cond LF@-1 int@0\n"                      \
        "JUMPIFEQ @substring_end LF@cond bool@true\n"   \
        "LT LF@cond LF@-2 int@0\n"                      \
        "JUMPIFEQ @substring_end LF@cond bool@true\n"   \
        "GT LF@cond LF@-1 LF@-2\n"                      \
        "JUMPIFEQ @substring_end LF@cond bool@true\n"   \
        "LT LF@cond LF@-1 LF@length\n"                  \
        "JUMPIFEQ @substring_end LF@cond bool@false\n"  \
        "GT LF@cond LF@-2 LF@length\n"                  \
        "JUMPIFEQ @substring_end LF@cond bool@true\n"   \
        "DEFVAR LF@index\n"                             \
        "MOVE LF@index LF@-1\n"                         \
        "DEFVAR LF@char\n"                              \
        "LABEL @substring_loop\n"                       \
        "GETCHAR LF@char LF@-0 LF@index\n"              \
        "CONCAT LF@res LF@res LF@char\n"                \
        "ADD LF@index LF@index int@1\n"                 \
        "LT LF@cond LF@index LF@-2\n"                   \
        "JUMPIFEQ @substring_loop LF@cond bool@true\n"  \
        "LABEL @substring_end\n"                        \
        "POPFRAME\n"                                    \
        "RETURN\n"

/// function ord(string $c) : int
#define FUNCTION_ORD                                \
        "# Function ord\n"                          \
        "LABEL @ord\n"                              \
        "PUSHFRAME\n"                               \
        "DEFVAR LF@res\n"                           \
        "MOVE LF@res int@0\n"                       \
        "DEFVAR LF@length\n"                        \
        "CREATEFRAME\n"                             \
        "DEFVAR TF@-0\n"                            \
        "MOVE TF@-0 LF@-0\n"                        \
        "CALL @strlen\n"                            \
        "MOVE LF@length TP@res\n"                   \
        "DEFVAR LF@cond\n"                          \
        "GT LF@cond LF@length int@0\n"              \
        "JUMPIFEQ @ord_end LF@cond bool@false\n"    \
        "STRI2INT LF@res LF@-0 int@0\n"             \
        "LABEL @ord_end\n"                          \
        "POPFRAME\n"                                \
        "RETURN\n"

/// function chr(int $i) : string
#define FUNCTION_CHR                            \
        "# Function chr\n"                      \
        "LABEL @chr\n"                          \
        "PUSHFRAME\n"                           \
        "DEFVAR LF@res\n"                       \
        "MOVE LF@res string@\n"                 \
        "DEFVAR LF@cond\n"                      \
        "GT LF@cond LF@-0 int@255\n"            \
        "JUMPIFEQ $chr_end LF@cond bool@true\n" \
        "LT LF@cond LF@-0 int@0\n"              \
        "JUMPIFEQ $chr_end LF@cond bool@true\n" \
        "INT2CHAR LF@res LF@-0\n"               \
        "LABEL @chr_end\n"                      \
        "POPFRAME\n"                            \
        "RETURN\n"


str_t code;


bool generate_header() {
    ADD_CODE_N(".IFJcode22\n"
               "DEFVAR GF@tmp_op1\n"
               "DEFVAR GF@tmp_op2\n"
               "DEFVAR GF@tmp_op3\n"
               "DEFVAR GF@exp_result\n"
               "JUMP @main");
    return true;
}


bool generate_built_in_funcs() {
    ADD_CODE(FUNCTION_READS);
    ADD_CODE(FUCNTION_READI);
    ADD_CODE(FUNCTION_READF);
    ADD_CODE(FUNCTION_STRLEN);
    ADD_CODE(FUNCTION_SUBSTRING);
    ADD_CODE(FUNCTION_ORD);
    ADD_CODE(FUNCTION_CHR);
    return true;
}


bool code_generator_start() {
    if (!str_const(&code)) return false;
    if (!generate_header()) return false;
    if (!generate_built_in_funcs()) return false;
    return true;
}


void code_generator_finish() {
    str_dest(&code);
}


void code_generator_flush(FILE* file) {
    fprintf(file, "%s", code.ptr);
    code_generator_finish();
}


bool generate_main_start() {
    ADD_CODE_N("# MAIN FUNCTION\n"
               "LABEL @main\n"
               "CREATEFRAME\n"
               "PUSHFRAME");
    return true;
}


bool generate_main_end() {
    ADD_CODE_N("# main end\n"
               "POPFRAME\n"
               "CLEARS");
    return true;
}


bool generate_function_start(char* name) {
    ADD_CODE("# Function ");
    ADD_CODE(name);
    ADD_CODE("\n");

    ADD_CODE("LABEL @");
    ADD_CODE(name);
    ADD_CODE("\n");

    ADD_CODE_N("PUSHFRAME");

    return true;
}


bool generate_function_end(char* name) {
    ADD_CODE("# ");
    ADD_CODE(name);
    ADD_CODE_N(" end");

    ADD_CODE("LABEL @");
    ADD_CODE(name);
    ADD_CODE_N("_end");

    ADD_CODE_N("POPFRAME");

    ADD_CODE_N("RETURN");

    return true;
}


bool generate_def_val(Data_type type) {
    switch (type) {
        case TYPE_FLOAT:
            ADD_CODE("float@0.0");
            break;

        case TYPE_INT:
            ADD_CODE("int@0");
            break;

        case TYPE_STRING:
            ADD_CODE("string@");
            break;

        case TYPE_BOOL:
            ADD_CODE("bool@false");
            break;

        case TYPE_NIL:
            ADD_CODE("nil@nil");
            break;

        default:
            return false;
    }
    return true;
}


bool generate_function_res(Data_type type) {
    ADD_CODE_N("DEFVAR LF@res");

    ADD_CODE("MOVE LF@res ");
    if (!generate_def_val(type)) return false;
    ADD_CODE("\n");

    return true;
}


bool generate_defvar(char* var) {
    ADD_CODE("DEFVAR LF@");
    ADD_CODE(var);
    ADD_CODE("\n");

    return true;
}


bool generate_var_def(Data_type type, char* var) {
    ADD_CODE("MOVE LF@");
    ADD_CODE(var);
    ADD_CODE(" ");
    if (!generate_def_val(type)) return false;
    ADD_CODE("\n");

    return true;
}


bool generate_function_call(char* name) {
    ADD_CODE("CALL @");
    ADD_CODE(name);
    ADD_CODE("\n");

    return true;
}


bool generate_function_res_assign(char* var, Data_type var_type, Data_type res_type) {
    if (var_type == TYPE_INT && res_type == TYPE_FLOAT) {
        ADD_CODE_N("FLOAT2INT TF@res TF@res");
    } else if (var_type == TYPE_FLOAT && res_type == TYPE_INT) {
        ADD_CODE_N("INT2FLOAT TF@res TF@res");
    }

    ADD_CODE("MOVE LF@");
    ADD_CODE(var);
    ADD_CODE_N(" TF@res");

    return true;
}


bool generate_function_param_declare(char* name, int64_t index) {
    ADD_CODE("DEFVAR LF@");
    ADD_CODE(name);
    ADD_CODE("\n");

    ADD_CODE("MOVE LF@");
    ADD_CODE(name);
    ADD_CODE(" LF@-");
    ADD_NUM(index);
    ADD_CODE("\n");

    return true;
}


bool generate_value_from_token(Token token) {
    char term[MAX_DIGITS];
    unsigned char c;
    str_t tmp;
    if (!str_const(&tmp)) return false;
    switch (token.type) {
        case token_integer:
            sprintf(term, "%d", token.value.integer);
            ADD_CODE("int@");
            ADD_CODE(term);
            break;

        case token_float:
            sprintf(term, "%f", token.value.decimal);
            ADD_CODE("float@");
            ADD_CODE(term);
            break;

        case token_string:
            for (int64_t i = 0; (c = (unsigned char) (token.value.String->ptr)[i]) != '\0'; i++) {
                if (c == '#' || c == '\\' || c <= 32 || !isprint(c)) {
                    str_add_sign(&tmp, '\\');
                    sprintf(term, "%03d", c);
                    str_concat(&tmp, term);
                } else {
                    str_add_sign(&tmp, (char) c);
                }
            }
            ADD_CODE("string@");
            ADD_CODE(tmp.ptr);
            break;

        case token_ID:
            ADD_CODE("LF@");
            ADD_CODE(token.value.String->ptr);
            break;

        default:
            str_dest(&tmp);
            return false;
    }
    str_dest(&tmp);
    return true;
}


bool generate_function_before_pass_params() {
    ADD_CODE_N("CREATEFRAME");

    return true;
}


bool generate_function_convert_passed_param(Data_type from, Data_type to, int64_t index) {
    if (to == TYPE_INT && from == TYPE_FLOAT) {
        ADD_CODE("FLOAT2INT TF@-");
        ADD_NUM(index);
        ADD_CODE(" TF@-");
        ADD_NUM(index);
        ADD_CODE("\n");
    } else if (to == TYPE_FLOAT && from == TYPE_INT) {
        ADD_CODE("INT2FLOAT TF@-");
        ADD_NUM(index);
        ADD_CODE(" TF@-");
        ADD_NUM(index);
        ADD_CODE("\n");
    }

    return true;
}


bool generate_function_pass_param(Token token, int64_t index) {
    ADD_CODE("DEFVAR TF@-");
    ADD_NUM(index);
    ADD_CODE("\n");

    ADD_CODE("MOVE TF@-");
    ADD_NUM(index);
    ADD_CODE(" ");
    if (!generate_value_from_token(token)) return false;
    ADD_CODE("\n");

    return true;
}


bool generate_function_return(char* name) {
    ADD_CODE_N("MOVE LF@res GF@exp_result");

    ADD_CODE("JUMP @");
    ADD_CODE(name);
    ADD_CODE_N("_end");

    return true;
}


bool generate_input(char* var, Data_type type) {
    ADD_CODE_N("WRITE GF@input_prompt");

    ADD_CODE("READ LF@");
    ADD_CODE(var);
    ADD_CODE(" ");
    switch (type) {
        case TYPE_FLOAT:
            ADD_CODE_N("float");
            break;

        case TYPE_INT:
            ADD_CODE_N("int");
            break;

        case TYPE_STRING:
            ADD_CODE_N("string");
            break;

        default:
            return false;
    }

    return true;
}


bool generate_exp_res() {
    ADD_CODE_N("WRITE GF@exp_result");

    return true;
}


bool generate_push(Token token) {
    ADD_CODE("PUSHS ");
    if (!generate_value_from_token(token)) return false;
    ADD_CODE("\n");

    return true;
}


bool generate_stack_operation(Rule_type rule) {
    switch (rule) {
        case RULE_ADD:
            ADD_CODE_N("ADDS");
            break;

        case RULE_SUB:
            ADD_CODE_N("SUBS");
            break;

        case RULE_DOT:
            ADD_CODE_N("POPS GF@tmp_op1\n"
                       "POPS GF@tmp_op2\n"
                       "CONCAT GF@tmp_op2 GF@tmp_op2 GF@tmp_op1\n"
                       "PUSHS GF@tmp_op2");
            break;

        case RULE_MUL:
            ADD_CODE_N("MULS");
            break;

        case RULE_DIV:
            ADD_CODE_N("DIVS");
            break;

        case RULE_EQ:
            ADD_CODE_N("EQS");
            break;

        case RULE_NEQ:
            ADD_CODE_N("EQS\n"
                       "NOTS");
            break;

        case RULE_LT:
            ADD_CODE_N("LTS");
            break;

        case RULE_GT:
            ADD_CODE_N("GTS");
            break;

        case RULE_LEQ:
            ADD_CODE_N("GTS\n"
                       "NOTS");
            break;

        case RULE_GEQ:
            ADD_CODE_N("LTS\n"
                       "NOTS");
            break;

        default:
            break;
    }
    return true;
}


bool generate_stack_concat() {
    ADD_CODE_N("POPS GF@tmp_op1\n"
               "POPS GF@tmp_op2\n"
               "CONCAT GF@tmp_op2 GF@tmp_op2 GF@tmp_op1\n"
               "PUSHS GF@tmp_op2");

    return true;
}


bool generate_stack_pop_res(char* var, Data_type res_type, Data_type var_type, char* frame) {
    if (var_type == TYPE_INT && res_type == TYPE_FLOAT) {
        ADD_CODE_N("FLOAT2INTS");
    } else if (var_type == TYPE_FLOAT && res_type == TYPE_INT) {
        ADD_CODE_N("INT2FLOATS");
    }

    ADD_CODE("POPS ");
    ADD_CODE(frame);
    ADD_CODE("@");
    ADD_CODE(var);
    ADD_CODE("\n");

    return true;
}


bool generate_stack_top_int2float() {
    ADD_CODE_N("INT2FLOATS");

    return true;
}


bool generate_stack_top_float2int() {
    ADD_CODE_N("FLOAT2INTS");

    return true;
}


bool generate_stack_sec_int2float() {
    ADD_CODE_N("POPS GF@tmp_op1\n"
               "INT2FLOATS\n"
               "PUSHS GF@tmp_op1");

    return true;
}


bool generate_stack_sec_float2int() {
    ADD_CODE_N("POPS GF@tmp_op1\n"
               "FLOAT2INTS\n"
               "PUSHS GF@tmp_op1");

    return true;
}


bool generate_label(char* name, int64_t deep, int64_t index) {
    ADD_CODE("LABEL @");
    ADD_CODE(name);
    ADD_CODE("_");
    ADD_NUM(deep);
    ADD_CODE("_");
    ADD_NUM(index);
    ADD_CODE("\n");

    return true;
}


bool generate_if_jump(char* name, int64_t deep, int64_t index) {
    ADD_CODE_N("# if");

    ADD_CODE("JUMPIGEQ @");
    ADD_CODE(name);
    ADD_CODE("_");
    ADD_NUM(deep);
    ADD_CODE("_");
    ADD_NUM(index);
    ADD_CODE_N(" GF@exp_result bool@false");

    return true;
}


bool generate_else_jump(char* name, int64_t deep, int64_t index) {
    ADD_CODE("JUMP @");
    ADD_CODE(name);
    ADD_CODE("_");
    ADD_NUM(deep);
    ADD_CODE("_");
    ADD_NUM(index + 1);
    ADD_CODE("\n");

    ADD_CODE_N("# else");

    if (!generate_label(name, deep, index)) return false;

    return true;
}


bool generate_if_end(char* name, int64_t deep, int64_t index) {
    ADD_CODE_N("# if end");

    if (!generate_label(name, deep, index)) return false;

    return true;
}


bool generate_while_head(char* name, int64_t deep, int64_t index) {
    ADD_CODE_N("# while");

    if(!generate_label(name, deep, index)) return false;

    return true;
}


bool generate_while_start(char* name, int64_t deep, int64_t index) {
    ADD_CODE("JUMPIFEQ @");
    ADD_CODE(name);
    ADD_CODE("_");
    ADD_NUM(deep);
    ADD_CODE("_");
    ADD_NUM(index);
    ADD_CODE_N(" GF@exp_result bool@false");

    return true;
}


bool generate_while_end(char* name, int64_t deep, int64_t index) {
    ADD_CODE("JUMP @");
    ADD_CODE(name);
    ADD_CODE("_");
    ADD_NUM(deep);
    ADD_CODE("_");
    ADD_NUM(index - 1);
    ADD_CODE("\n");

    ADD_CODE_N("# loop");

    if(!generate_label(name, deep, index)) return false;

    return true;
}