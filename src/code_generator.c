#include "code_generator.h"
#include "macros.h"
#include "builtins.h"


str_t g_Code;
FILE* g_CodegenOut = NULL;



bool emit_header() {
    EMIT_NL(".IFJcode22\n"
           "DEFVAR GF@$TMP_REG1\n"
           "DEFVAR GF@$TMP_REG2\n"
           "DEFVAR GF@$TMP_REG3\n"
           "DEFVAR GF@$EXPR_REG\n"
           "JUMP !program_body\n");
    return true;
}

bool emit_built_in_funcs() {
    EMIT(FUNCTION_READS);
    EMIT(FUCNTION_READI);
    EMIT(FUNCTION_READF);
    EMIT(FUNCTION_WRITE);
    EMIT(FUNCTION_FLOATVAL);
    EMIT(FUNCTION_INTVAL);
    EMIT(FUNCTION_STRVAL);
    EMIT(FUNCTION_STRLEN);
    EMIT(FUNCTION_SUBSTRING);
    EMIT(FUNCTION_ORD);
    EMIT(FUNCTION_CHR);
    return true;
}


bool code_generator_init() {
    if (!str_const(&g_Code)) return false;
    if (!emit_header()) return false;
    if (!emit_built_in_funcs()) return false;
    return true;
}


void code_generator_terminate() {
    str_dest(&g_Code);
}


void code_generator_flush(FILE* file) {
    fprintf(file, "%s", g_Code.ptr);
    str_clear(&g_Code);
    //code_generator_finish();
}

bool emit_clear_stack()
{
    EMIT_NL("CLEARS");
    return true;
}

bool emit_program_body_open()
{
    EMIT_NL("LABEL !program_body");
    return true;
}

bool emit_program_body_close()
{
    EMIT_NL("CLEARS");
    return true;
}

bool emit_push_bool_literal(bool value)
{
    EMIT("PUSHS bool@");
    EMIT(value ? "true" : "false");
    EMIT("\n");
    return true;
}

bool emit_function_open(const char* name) 
{
    EMIT("JUMP !");
    EMIT(name);
    EMIT_NL("_skip");

    EMIT("\n# Function ");
    EMIT(name);
    EMIT("\n");

    EMIT("LABEL !");
    EMIT(name);
    EMIT("\n");

    //EMIT_NL("CREATEFRAME");
    EMIT_NL("PUSHFRAME");

    return true;
}


bool emit_function_close(const char* name) 
{
    EMIT_NL("POPFRAME");

    EMIT_NL("RETURN\n");

    EMIT("LABEL !");
    EMIT(name);
    EMIT_NL("_skip");

    return true;
}


bool emit_def_val(DataType type) {
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

        case TYPE_NULL:
            EMIT("nil@nil");
            break;

        default:
            return false;
    }
    return true;
}


bool emit_function_res(DataType type) {
    EMIT_NL("DEFVAR LF@res");

    EMIT("MOVE LF@res ");
    if (!emit_def_val(type)) return false;
    EMIT("\n");

    return true;
}


bool emit_define_var(const char* var, bool in_local_scope) {
    EMIT("DEFVAR ");
    const char* frame = in_local_scope ? "LF" : "GF";
    EMIT(frame);
    EMIT("@");
    EMIT(var);
    EMIT("\n");

    return true;
}


// bool emit_var_def(DataType type, char* var) {
//     EMIT("MOVE LF@");
//     EMIT(var);
//     EMIT(" ");
//     if (!emit_def_val(type)) return false;
//     EMIT("\n");

//     return true;
// }


bool emit_function_call(const char* name) {
    EMIT("CALL !");
    EMIT(name);
    EMIT("\n");

    return true;
}


bool emit_function_res_assign(const char* var_name, bool local_frame)
{
    // if (var_type == TYPE_INT && res_type == TYPE_FLOAT) {
    //     EMIT_NL("FLOAT2INT TF@res TF@res");
    // } else if (var_type == TYPE_FLOAT && res_type == TYPE_INT) {
    //     EMIT_NL("INT2FLOAT TF@res TF@res");
    // }

    EMIT("MOVE ");
    EMIT(local_frame ? "LF@" : "GF@");
    EMIT(var_name);
    EMIT_NL(" TF@res");

    return true;
}


bool emit_function_param_declare(const char* name, int64_t index) {
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


bool emit_value_from_token(Token token, bool local_frame) 
{
    char term[MAX_DIGITS];
    unsigned char c;
    str_t tmp;
    if (!str_const(&tmp)) return false;
    switch (token.type) {
        case token_integer:
            snprintf(term, MAX_DIGITS, "%d", token.integer);
            EMIT("int@");
            EMIT(term);
            break;

        case token_float:
            snprintf(term, MAX_DIGITS, "%a", token.decimal);
            EMIT("float@");
            EMIT(term);
            break;

        case token_string:
            for (int64_t i = 0; (c = (unsigned char) (token.string.ptr)[i]) != '\0'; i++) {
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
        case token_null:
            EMIT("nil@nil");
            break;
        case token_ID:
            EMIT(local_frame ? "LF@" : "GF@");
            EMIT(token.string.ptr);
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


// bool emit_function_convert_passed_param(DataType from, DataType to, int64_t index) {
//     if (to == TYPE_INT && from == TYPE_FLOAT) {
//         EMIT("FLOAT2INT TF@-");
//         EMIT_INT(index);
//         EMIT(" TF@-");
//         EMIT_INT(index);
//         EMIT("\n");
//     } else if (to == TYPE_FLOAT && from == TYPE_INT) {
//         EMIT("INT2FLOAT TF@-");
//         EMIT_INT(index);
//         EMIT(" TF@-");
//         EMIT_INT(index);
//         EMIT("\n");
//     }

//     return true;
// }


bool emit_function_pass_param(Token token, int64_t index, bool local_frame) {
    EMIT("DEFVAR TF@-");
    EMIT_INT(index);
    EMIT("\n");

    EMIT("MOVE TF@-");
    EMIT_INT(index);
    EMIT(" ");
    if (!emit_value_from_token(token, local_frame)) return false;
    EMIT("\n");

    return true;
}


bool emit_function_pass_param_push(Token token, bool local_frame) {
    EMIT("PUSHS ");
    if (!emit_value_from_token(token, local_frame)) return false;
    EMIT("\n");

    return true;
}


bool emit_function_pass_param_count(int64_t count)
{
    EMIT("DEFVAR TF@arg_count\n");
    EMIT("MOVE TF@arg_count int@");
    EMIT_INT(count);
    EMIT("\n");

    return true;
}


bool emit_function_return(const char* name) {
    EMIT_NL("MOVE LF@res GF@$EXPR_REG");

    EMIT("JUMP !");
    EMIT(name);
    EMIT_NL("_end");

    return true;
}


bool emit_bool_value(bool value)
{
    EMIT("bool@");
    EMIT(value ? "true" : "false");
    return true;
}


bool emit_exp_res() {
    EMIT_NL("WRITE GF@$EXPR_REG");

    return true;
}


bool emit_push(Token token, bool local_frame) {
    EMIT("PUSHS ");
    if (!emit_value_from_token(token, local_frame)) return false;
    EMIT("\n");

    return true;
}


bool emit_pop()
{
    EMIT_NL("POPS GF@$TMP_REG1");
    return true;
}

//static int64_t s_EqCounter = 0;

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
            // char str[MAX_DIGITS];
            // sprintf(str, "%d", s_EqCounter);
            // EMIT_NL("POPS GF@$TMP_REG1\n"
            //         "TYPE GF@$TMP_REG1 GF@$TMP_REG1\n"
            //         "POPS GF@$TMP_REG2\n"
            //         "TYPE GF@$TMP_REG2 GF@$TMP_REG2\n"
            //         "POPS GF@$TMP_REG2\n"
            //         "JUMPIFNEQ  GF@$TMP_REG1 GF@$TMP_REG2");
            // EMIT("LABEL !eq");
            // EMIT(str); EMIT("\n");
            


            EMIT_NL("EQS");
            // ++s_EqCounter;
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


// bool emit_stack_pop_res(const char* var, DataType res_type, DataType var_type, const char* frame) {
//     if (var_type == TYPE_INT && res_type == TYPE_FLOAT) {
//         EMIT_NL("FLOAT2INTS");
//     } else if (var_type == TYPE_FLOAT && res_type == TYPE_INT) {
//         EMIT_NL("INT2FLOATS");
//     } //TODO: TYPE_BOOL ??

//     EMIT("POPS ");
//     EMIT(frame);
//     EMIT("@");
//     EMIT(var);
//     EMIT("\n");

//     return true;
// }

bool emit_stack_pop_res(const char* var, const char* frame) 
{
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


bool emit_label(const char* name, int64_t deep, int64_t index) {
    EMIT("LABEL !");
    EMIT(name);
    EMIT("_");
    EMIT_INT(deep);
    EMIT("_");
    EMIT_INT(index);
    EMIT("\n");

    return true;
}


bool emit_if_head()
{
    EMIT_NL("\n# if");

    return true;
}

bool emit_if_open(const char* name, int64_t deep, int64_t index) {
    EMIT("JUMPIFEQ !");
    EMIT(name);
    EMIT("_");
    EMIT_INT(deep);
    EMIT("_");
    EMIT_INT(index);
    EMIT_NL(" GF@$EXPR_REG bool@false");

    return true;
}


bool emit_else(const char* name, int64_t deep, int64_t index) {
    EMIT("JUMP !");
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


bool emit_if_close(const char* name, int64_t deep, int64_t index) {
    EMIT_NL("# if end");
    if (!emit_label(name, deep, index)) return false;
    EMIT("\n");

    return true;
}


bool emit_while_head(const char* name, int64_t deep, int64_t index) {
    EMIT_NL("\n# while");

    if(!emit_label(name, deep, index)) return false;

    return true;
}


bool emit_while_open(const char* name, int64_t deep, int64_t index) {
    EMIT("JUMPIFEQ !");
    EMIT(name);
    EMIT("_");
    EMIT_INT(deep);
    EMIT("_");
    EMIT_INT(index);
    EMIT_NL(" GF@$EXPR_REG bool@false");

    return true;
}


bool emit_while_close(const char* name, int64_t deep, int64_t index) {
    EMIT("JUMP !");
    EMIT(name);
    EMIT("_");
    EMIT_INT(deep);
    EMIT("_");


    EMIT_INT(index - 1);
    
    
    EMIT("\n");


    EMIT_NL("# loop");
    if(!emit_label(name, deep, index)) return false;
    EMIT("\n");

    return true;
}