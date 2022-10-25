#include "code_generator.h"

#define EMIT(text)                              \
    if (!str_concat(&code, (text))) return false

#define EMIT_N(text)                            \
    if (!str_concat(&code, (text"\n"))) return false

#define EMIT_NUM(number)                 \
    do {                                \
        char str[MAX_DIGITS];           \
        sprintf(str, "%ld", (number));  \
        EMIT(str);                  \
    } while (0)

#define MAX_DIGITS 50


/**
* GENERATOR OF BUILT-IN FUNCTIONS
*/
#define BFNEND "\n"
/// function reads() : ?string
#define FUNCTION_READS              \
        "# Function reads\n"        \
        "LABEL @reads\n"            \
        "PUSHFRAME\n"               \
        "DEFVAR LF@res\n"           \
        "READ LF@res string\n"      \
        "POPFRAME\n"                \
        "RETURN\n" BFNEND

/// function readi() : ?int
#define FUCNTION_READI          \
        "# Function readi\n"    \
        "LABEL @readi\n"        \
        "PUSHFRAME\n"           \
        "DEFVAR LF@res\n"       \
        "READ LF@res int\n"     \
        "POPFRAME\n"            \
        "RETURN\n" BFNEND

/// function readf() : ?float
#define FUNCTION_READF              \
        "# Function readf\n"        \
        "LABEL @readf\n"            \
        "PUSHFRAME\n"               \
        "DEFVAR LF@res\n"           \
        "READ LF@res float\n"       \
        "POPFRAME\n"                \
        "RETURN\n" BFNEND

/// function strlen(string $𝑠) : int
#define FUNCTION_STRLEN                 \
        "# Function strlen\n"           \
        "LABEL @strlen\n"               \
        "PUSHFRAME\n"                   \
        "DEFVAR LF@res\n"               \
        "STRLEN LF@res LF@-0\n"         \
        "POPFRAME\n"                    \
        "RETURN\n" BFNEND

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
        "RETURN\n" BFNEND

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
        "RETURN\n" BFNEND

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
        "RETURN\n" BFNEND


static str_t code;


bool emit_header() {
    EMIT_N(".IFJcode22\n"
               "DEFVAR GF@tmp_op1\n"
               "DEFVAR GF@tmp_op2\n"
               "DEFVAR GF@tmp_op3\n"
               "DEFVAR GF@exp_result\n");
    return true;
}


bool emit_built_in_funcs() {
    EMIT(FUNCTION_READS);
    EMIT(FUCNTION_READI);
    EMIT(FUNCTION_READF);
    EMIT(FUNCTION_STRLEN);
    EMIT(FUNCTION_SUBSTRING);
    EMIT(FUNCTION_ORD);
    EMIT(FUNCTION_CHR);
    return true;
}


bool code_generator_start() {
    if (!str_const(&code)) return false;
    if (!emit_header()) return false;
    //if (!emit_built_in_funcs()) return false;
    return true;
}


void code_generator_finish() {
    str_dest(&code);
}


void code_generator_flush(FILE* file) {
    fprintf(file, "%s", code.ptr);
    code_generator_finish();
}


bool emit_body_start() {
    EMIT_N("# PROGRAM BODY\n"
               "CREATEFRAME\n"
               "PUSHFRAME");
    return true;
}


bool emit_body_end() {
    EMIT_N("# main end\n"
               "POPFRAME\n"
               "CLEARS");
    return true;
}


bool emit_function_start(char* name) {
    EMIT("# Function ");
    EMIT(name);
    EMIT("\n");

    EMIT("LABEL @");
    EMIT(name);
    EMIT("\n");

    EMIT_N("PUSHFRAME");

    return true;
}


bool emit_function_end(char* name) {
    EMIT("# ");
    EMIT(name);
    EMIT_N(" end");

    EMIT("LABEL @");
    EMIT(name);
    EMIT_N("_end");

    EMIT_N("POPFRAME");

    EMIT_N("RETURN");

    return true;
}


bool emit_def_val(Data_type type) {
    switch (type) {
        case TYPE_FLOAT:
            EMIT("float@0.0");
            break;

        case TYPE_INT:
            EMIT("int@0");
            break;

        case TYPE_STRING:
            EMIT("string@");
            break;

        case TYPE_BOOL:
            EMIT("bool@false");
            break;

        case TYPE_NIL:
            EMIT("nil@nil");
            break;

        default:
            return false;
    }
    return true;
}


bool emit_function_res(Data_type type) {
    EMIT_N("DEFVAR LF@res");

    EMIT("MOVE LF@res ");
    if (!emit_def_val(type)) return false;
    EMIT("\n");

    return true;
}


bool emit_defvar(char* var) {
    EMIT("DEFVAR LF@");
    EMIT(var);
    EMIT("\n");

    return true;
}


bool emit_var_def(Data_type type, char* var) {
    EMIT("MOVE LF@");
    EMIT(var);
    EMIT(" ");
    if (!emit_def_val(type)) return false;
    EMIT("\n");

    return true;
}


bool emit_function_call(char* name) {
    EMIT("CALL @");
    EMIT(name);
    EMIT("\n");

    return true;
}


bool emit_function_res_assign(char* var, Data_type var_type, Data_type res_type) {
    if (var_type == TYPE_INT && res_type == TYPE_FLOAT) {
        EMIT_N("FLOAT2INT TF@res TF@res");
    } else if (var_type == TYPE_FLOAT && res_type == TYPE_INT) {
        EMIT_N("INT2FLOAT TF@res TF@res");
    }

    EMIT("MOVE LF@");
    EMIT(var);
    EMIT_N(" TF@res");

    return true;
}


bool emit_function_param_declare(char* name, int64_t index) {
    EMIT("DEFVAR LF@");
    EMIT(name);
    EMIT("\n");

    EMIT("MOVE LF@");
    EMIT(name);
    EMIT(" LF@-");
    EMIT_NUM(index);
    EMIT("\n");

    return true;
}


bool emit_value_from_token(Token token) {
    char term[MAX_DIGITS];
    unsigned char c;
    str_t tmp;
    if (!str_const(&tmp)) return false;
    switch (token.type) {
        case token_integer:
            sprintf(term, "%d", token.value.integer);
            EMIT("int@");
            EMIT(term);
            break;

        case token_float:
            sprintf(term, "%f", token.value.decimal);
            EMIT("float@");
            EMIT(term);
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
            EMIT("string@");
            EMIT(tmp.ptr);
            break;

        case token_ID:
            EMIT("LF@");
            EMIT(token.value.String->ptr);
            break;

        default:
            str_dest(&tmp);
            return false;
    }
    str_dest(&tmp);
    return true;
}


bool emit_function_before_pass_params() {
    EMIT_N("CREATEFRAME");

    return true;
}


bool emit_function_convert_passed_param(Data_type from, Data_type to, int64_t index) {
    if (to == TYPE_INT && from == TYPE_FLOAT) {
        EMIT("FLOAT2INT TF@-");
        EMIT_NUM(index);
        EMIT(" TF@-");
        EMIT_NUM(index);
        EMIT("\n");
    } else if (to == TYPE_FLOAT && from == TYPE_INT) {
        EMIT("INT2FLOAT TF@-");
        EMIT_NUM(index);
        EMIT(" TF@-");
        EMIT_NUM(index);
        EMIT("\n");
    }

    return true;
}


bool emit_function_pass_param(Token token, int64_t index) {
    EMIT("DEFVAR TF@-");
    EMIT_NUM(index);
    EMIT("\n");

    EMIT("MOVE TF@-");
    EMIT_NUM(index);
    EMIT(" ");
    if (!emit_value_from_token(token)) return false;
    EMIT("\n");

    return true;
}


bool emit_function_return(char* name) {
    EMIT_N("MOVE LF@res GF@exp_result");

    EMIT("JUMP @");
    EMIT(name);
    EMIT_N("_end");

    return true;
}


bool emit_input(char* var, Data_type type) {
    EMIT_N("WRITE GF@input_prompt");

    EMIT("READ LF@");
    EMIT(var);
    EMIT(" ");
    switch (type) {
        case TYPE_FLOAT:
            EMIT_N("float");
            break;

        case TYPE_INT:
            EMIT_N("int");
            break;

        case TYPE_STRING:
            EMIT_N("string");
            break;

        default:
            return false;
    }

    return true;
}


bool emit_exp_res() {
    EMIT_N("WRITE GF@exp_result");

    return true;
}


bool emit_push(Token token) {
    EMIT("PUSHS ");
    if (!emit_value_from_token(token)) return false;
    EMIT("\n");

    return true;
}


bool emit_stack_operation(Rule_type rule) {
    switch (rule) {
        case RULE_ADD:
            EMIT_N("ADDS");
            break;

        case RULE_SUB:
            EMIT_N("SUBS");
            break;

        case RULE_DOT:
            EMIT_N("POPS GF@tmp_op3\n"
                    "POPS GF@tmp_op2\n"
                    "CONCAT GF@tmp_op1 GF@tmp_op2 GF@tmp_op3\n"
                    "PUSHS GF@tmp_op1");
            break;

        case RULE_MUL:
            EMIT_N("MULS");
            break;

        case RULE_DIV:
            EMIT_N("DIVS");
            break;

        case RULE_EQ:
            EMIT_N("EQS");
            break;

        case RULE_NEQ:
            EMIT_N("EQS\n"
                       "NOTS");
            break;

        case RULE_LT:
            EMIT_N("LTS");
            break;

        case RULE_GT:
            EMIT_N("GTS");
            break;

        case RULE_LEQ:
            EMIT_N("GTS\n"
                       "NOTS");
            break;

        case RULE_GEQ:
            EMIT_N("LTS\n"
                       "NOTS");
            break;

        default:
            break;
    }
    return true;
}


bool emit_stack_concat() {
    EMIT_N("POPS GF@tmp_op1\n"
            "POPS GF@tmp_op2\n"
            "CONCAT GF@tmp_op2 GF@tmp_op2 GF@tmp_op1\n"
            "PUSHS GF@tmp_op2");

    return true;
}


bool emit_stack_pop_res(char* var, Data_type res_type, Data_type var_type, char* frame) {
    if (var_type == TYPE_INT && res_type == TYPE_FLOAT) {
        EMIT_N("FLOAT2INTS");
    } else if (var_type == TYPE_FLOAT && res_type == TYPE_INT) {
        EMIT_N("INT2FLOATS");
    }

    EMIT("POPS ");
    EMIT(frame);
    EMIT("@");
    EMIT(var);
    EMIT("\n");

    return true;
}


bool emit_stack_top_int2float() {
    EMIT_N("INT2FLOATS");

    return true;
}


bool emit_stack_top_float2int() {
    EMIT_N("FLOAT2INTS");

    return true;
}


bool emit_stack_sec_int2float() {
    EMIT_N("POPS GF@tmp_op1\n"
            "INT2FLOATS\n"
            "PUSHS GF@tmp_op1");

    return true;
}


bool emit_stack_sec_float2int() {
    EMIT_N("POPS GF@tmp_op1\n"
            "FLOAT2INTS\n"
            "PUSHS GF@tmp_op1");

    return true;
}


bool emit_label(char* name, int64_t deep, int64_t index) {
    EMIT("LABEL @");
    EMIT(name);
    EMIT("_");
    EMIT_NUM(deep);
    EMIT("_");
    EMIT_NUM(index);
    EMIT("\n");

    return true;
}


bool emit_if_jump(char* name, int64_t deep, int64_t index) {
    EMIT_N("# if");

    EMIT("JUMPIGEQ @");
    EMIT(name);
    EMIT("_");
    EMIT_NUM(deep);
    EMIT("_");
    EMIT_NUM(index);
    EMIT_N(" GF@exp_result bool@false");

    return true;
}


bool emit_else_jump(char* name, int64_t deep, int64_t index) {
    EMIT("JUMP @");
    EMIT(name);
    EMIT("_");
    EMIT_NUM(deep);
    EMIT("_");
    EMIT_NUM(index + 1);
    EMIT("\n");

    EMIT_N("# else");

    if (!emit_label(name, deep, index)) return false;

    return true;
}


bool emit_if_end(char* name, int64_t deep, int64_t index) {
    EMIT_N("# if end");

    if (!emit_label(name, deep, index)) return false;

    return true;
}


bool emit_while_head(char* name, int64_t deep, int64_t index) {
    EMIT_N("# while");

    if(!emit_label(name, deep, index)) return false;

    return true;
}


bool emit_while_start(char* name, int64_t deep, int64_t index) {
    EMIT("JUMPIFEQ @");
    EMIT(name);
    EMIT("_");
    EMIT_NUM(deep);
    EMIT("_");
    EMIT_NUM(index);
    EMIT_N(" GF@exp_result bool@false");

    return true;
}


bool emit_while_end(char* name, int64_t deep, int64_t index) {
    EMIT("JUMP @");
    EMIT(name);
    EMIT("_");
    EMIT_NUM(deep);
    EMIT("_");
    EMIT_NUM(index - 1);
    EMIT("\n");

    EMIT_N("# loop");

    if(!emit_label(name, deep, index)) return false;

    return true;
}