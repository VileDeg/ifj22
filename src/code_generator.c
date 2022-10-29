#include "code_generator.h"

#define EMIT(text)                              \
    if (!str_concat(&code, (text))) return false

#define EMIT_NL(text)                            \
    if (!str_concat(&code, (text"\n"))) return false

#define EMIT_INT(number)                \
    do {                                \
        char str[MAX_DIGITS];           \
        sprintf(str, "%ld", (number));  \
        EMIT(str);                      \
    } while (0)

#define MAX_DIGITS 50

#if 1
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

/// function write ( term1 , term2 , ..., term𝑛 ) : void
#define FUNCTION_WRITE                              \
        "# Function write\n"                        \
        "LABEL @write\n"                            \
        "PUSHFRAME\n"                               \
        "DEFVAR LF@to_write\n"                      \
        "DEFVAR LF@type\n"                          \
        "TYPE LF@type LF@arg_count\n"                  \
        "JUMPIFNEQ @write_end LF@type string@int\n" \
        "DEFVAR LF@cond\n"                          \
        "LT LF@cond LF@arg_count int@1\n"              \
        "JUMPIFEQ @write_end LF@cond bool@true\n"   \
        "LABEL @write_loop\n"                       \
        "POPS @to_write\n"                          \
        "WRITE @to_write\n"                         \
        "SUB LF@arg_count LF@arg_count int@1\n"           \
        "GT LF@cond LF@arg_count int@0\n"              \
        "JUMPIFEQ @write_loop LF@cond bool@true\n"  \
        "LABEL @write_end\n"                        \
        "POPFRAME\n"                                \
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

#endif
static str_t code;


bool emit_header() {
    EMIT_NL(".IFJcode22\n"
           "DEFVAR GF@$TMP_REG1\n"
           "DEFVAR GF@$TMP_REG2\n"
           "DEFVAR GF@$TMP_REG3\n"
           "DEFVAR GF@$EXPR_REG\n");
    return true;
}


bool emit_built_in_funcs() {
    EMIT(FUNCTION_READS);
    EMIT(FUCNTION_READI);
    EMIT(FUNCTION_READF);
    EMIT(FUNCTION_WRITE);
    EMIT(FUNCTION_STRLEN);
    EMIT(FUNCTION_SUBSTRING);
    EMIT(FUNCTION_ORD);
    EMIT(FUNCTION_CHR);
    return true;
}


bool code_generator_start() {
    if (!str_const(&code)) return false;
    if (!emit_header()) return false;
    if (!emit_built_in_funcs()) return false;
    return true;
}


void code_generator_finish() {
    str_dest(&code);
}


void code_generator_flush(FILE* file) {
    fprintf(file, "%s", code.ptr);
    str_clear(&code);
    //code_generator_finish();
}


// bool emit_body_open() {
//     EMIT_NL("# PROGRAM BODY\n"
//            "CREATEFRAME\n"
//            "PUSHFRAME");
//     return true;
// }


// bool emit_body_close() {
//     EMIT_NL("# PROGRAM END\n"
//            "POPFRAME\n"
//            "CLEARS");
//     return true;
// }


bool emit_function_open(char* name) {
    EMIT("# Function ");
    EMIT(name);
    EMIT("\n");

    EMIT("LABEL @");
    EMIT(name);
    EMIT("\n");

    //EMIT_NL("PUSHFRAME");
    EMIT_NL("CREATEFRAME");
    EMIT_NL("PUSHFRAME");

    return true;
}


bool emit_function_close(char* name) {
    // EMIT("# ");
    // EMIT(name);
    // EMIT_NL(" end");

    // EMIT("LABEL @");
    // EMIT(name);
    // EMIT_NL("_end");

    EMIT_NL("POPFRAME");

    EMIT_NL("RETURN");

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
    EMIT_NL("DEFVAR LF@res");

    EMIT("MOVE LF@res ");
    if (!emit_def_val(type)) return false;
    EMIT("\n");

    return true;
}


bool emit_define_var(char* var, bool in_local_scope) {
    EMIT("DEFVAR ");
    const char* frame = in_local_scope ? "LF" : "GF";
    EMIT(frame);
    EMIT("@");
    EMIT(var);
    EMIT("\n");

    return true;
}


// bool emit_var_def(Data_type type, char* var) {
//     EMIT("MOVE LF@");
//     EMIT(var);
//     EMIT(" ");
//     if (!emit_def_val(type)) return false;
//     EMIT("\n");

//     return true;
// }


bool emit_function_call(char* name) {
    EMIT("CALL @");
    EMIT(name);
    EMIT("\n");

    return true;
}


bool emit_function_res_assign(char* var, Data_type var_type, Data_type res_type) {
    if (var_type == TYPE_INT && res_type == TYPE_FLOAT) {
        EMIT_NL("FLOAT2INT TF@res TF@res");
    } else if (var_type == TYPE_FLOAT && res_type == TYPE_INT) {
        EMIT_NL("INT2FLOAT TF@res TF@res");
    }

    EMIT("MOVE LF@");
    EMIT(var);
    EMIT_NL(" TF@res");

    return true;
}


bool emit_function_param_declare(char* name, int64_t index) {
    EMIT("DEFVAR LF@");
    EMIT(name);
    EMIT("\n");

    EMIT("MOVE LF@");
    EMIT(name);
    EMIT(" LF@-");
    EMIT_INT(index);
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
    EMIT_NL("CREATEFRAME");

    return true;
}


bool emit_function_convert_passed_param(Data_type from, Data_type to, int64_t index) {
    if (to == TYPE_INT && from == TYPE_FLOAT) {
        EMIT("FLOAT2INT TF@-");
        EMIT_INT(index);
        EMIT(" TF@-");
        EMIT_INT(index);
        EMIT("\n");
    } else if (to == TYPE_FLOAT && from == TYPE_INT) {
        EMIT("INT2FLOAT TF@-");
        EMIT_INT(index);
        EMIT(" TF@-");
        EMIT_INT(index);
        EMIT("\n");
    }

    return true;
}


bool emit_function_pass_param(Token token, int64_t index) {
    EMIT("DEFVAR TF@-");
    EMIT_INT(index);
    EMIT("\n");

    EMIT("MOVE TF@-");
    EMIT_INT(index);
    EMIT(" ");
    if (!emit_value_from_token(token)) return false;
    EMIT("\n");

    return true;
}


bool emit_function_pass_param_push(Token token) {
    EMIT("PUSHS ");
    if (!emit_value_from_token(token)) return false;
    EMIT("\n");

    return true;
}


bool emit_function_pass_param_count(int64_t count)
{
    EMIT("DEFVAR TF@arg_count\n");
    EMIT("MOVE TF@arg_count ");
    EMIT_INT(count);
    EMIT("\n");

    return true;
}


bool emit_function_return(char* name) {
    EMIT_NL("MOVE LF@res GF@$EXPR_REG");

    EMIT("JUMP @");
    EMIT(name);
    EMIT_NL("_end");

    return true;
}


bool emit_input(char* var, Data_type type) {
    EMIT_NL("WRITE GF@input_prompt");

    EMIT("READ LF@");
    EMIT(var);
    EMIT(" ");
    switch (type) {
        case TYPE_FLOAT:
            EMIT_NL("float");
            break;

        case TYPE_INT:
            EMIT_NL("int");
            break;

        case TYPE_STRING:
            EMIT_NL("string");
            break;

        default:
            return false;
    }

    return true;
}


bool emit_exp_res() {
    EMIT_NL("WRITE GF@$EXPR_REG");

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
            EMIT_NL("ADDS");
            break;

        case RULE_SUB:
            EMIT_NL("SUBS");
            break;

        case RULE_DOT:
            EMIT_NL("POPS GF@$TMP_REG3\n"
                    "POPS GF@$TMP_REG2\n"
                    "CONCAT GF@$TMP_REG1 GF@$TMP_REG2 GF@$TMP_REG3\n"
                    "PUSHS GF@$TMP_REG1");
            break;

        case RULE_MUL:
            EMIT_NL("MULS");
            break;

        case RULE_DIV:
            EMIT_NL("DIVS");
            break;

        case RULE_EQ:
            EMIT_NL("EQS");
            break;

        case RULE_NEQ:
            EMIT_NL("EQS\n"
                       "NOTS");
            break;

        case RULE_LT:
            EMIT_NL("LTS");
            break;

        case RULE_GT:
            EMIT_NL("GTS");
            break;

        case RULE_LEQ:
            EMIT_NL("GTS\n"
                       "NOTS");
            break;

        case RULE_GEQ:
            EMIT_NL("LTS\n"
                       "NOTS");
            break;

        default:
            break;
    }
    return true;
}


bool emit_stack_concat() {
    EMIT_NL("POPS GF@$TMP_REG1\n"
           "POPS GF@$TMP_REG2\n"
           "CONCAT GF@$TMP_REG2 GF@$TMP_REG2 GF@$TMP_REG1\n"
           "PUSHS GF@$TMP_REG2");

    return true;
}


bool emit_stack_pop_res(char* var, Data_type res_type, Data_type var_type, char* frame) {
    if (var_type == TYPE_INT && res_type == TYPE_FLOAT) {
        EMIT_NL("FLOAT2INTS");
    } else if (var_type == TYPE_FLOAT && res_type == TYPE_INT) {
        EMIT_NL("INT2FLOATS");
    }

    EMIT("POPS ");
    EMIT(frame);
    EMIT("@");
    EMIT(var);
    EMIT("\n");

    return true;
}


bool emit_stack_top_int2float() {
    EMIT_NL("INT2FLOATS");

    return true;
}


bool emit_stack_top_float2int() {
    EMIT_NL("FLOAT2INTS");

    return true;
}


bool emit_stack_sec_int2float() {
    EMIT_NL("POPS GF@$TMP_REG1\n"
           "INT2FLOATS\n"
           "PUSHS GF@$TMP_REG1");

    return true;
}


bool emit_stack_sec_float2int() {
    EMIT_NL("POPS GF@$TMP_REG1\n"
           "FLOAT2INTS\n"
           "PUSHS GF@$TMP_REG1");

    return true;
}


bool emit_label(char* name, int64_t deep, int64_t index) {
    EMIT("LABEL @");
    EMIT(name);
    EMIT("_");
    EMIT_INT(deep);
    EMIT("_");
    EMIT_INT(index);
    EMIT("\n");

    return true;
}


bool emit_if_jump(char* name, int64_t deep, int64_t index) {
    EMIT_NL("# if");

    EMIT("JUMPIGEQ @");
    EMIT(name);
    EMIT("_");
    EMIT_INT(deep);
    EMIT("_");
    EMIT_INT(index);
    EMIT_NL(" GF@$EXPR_REG bool@false");

    return true;
}


bool emit_else_jump(char* name, int64_t deep, int64_t index) {
    EMIT("JUMP @");
    EMIT(name);
    EMIT("_");
    EMIT_INT(deep);
    EMIT("_");
    EMIT_INT(index + 1);
    EMIT("\n");

    EMIT_NL("# else");

    if (!emit_label(name, deep, index)) return false;

    return true;
}


bool emit_if_end(char* name, int64_t deep, int64_t index) {
    EMIT_NL("# if end");

    if (!emit_label(name, deep, index)) return false;

    return true;
}


bool emit_while_head(char* name, int64_t deep, int64_t index) {
    EMIT_NL("# while");

    if(!emit_label(name, deep, index)) return false;

    return true;
}


bool emit_while_open(char* name, int64_t deep, int64_t index) {
    EMIT("JUMPIFEQ @");
    EMIT(name);
    EMIT("_");
    EMIT_INT(deep);
    EMIT("_");
    EMIT_INT(index);
    EMIT_NL(" GF@$EXPR_REG bool@false");

    return true;
}


bool emit_while_close(char* name, int64_t deep, int64_t index) {
    EMIT("JUMP @");
    EMIT(name);
    EMIT("_");
    EMIT_INT(deep);
    EMIT("_");
    EMIT_INT(index - 1);
    EMIT("\n");

    EMIT_NL("# loop");

    if(!emit_label(name, deep, index)) return false;

    return true;
}