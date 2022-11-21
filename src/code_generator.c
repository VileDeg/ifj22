#include "code_generator.h"
#include "builtins.h"
#include "internals.h"

str_t g_Code;
FILE* g_CodegenOut = NULL;

#define _FLBEXT ""

bool emit_header() {
    EMIT_NL(".IFJcode22\n"
           "DEFVAR GF@TMP_REG1\n"
           "DEFVAR GF@TMP_REG\n"
           "DEFVAR GF@TMP_REG3\n"
           "DEFVAR GF@EXPR_VAL\n"
           "DEFVAR GF@EXPR_TYPE\n"
           "JUMP !program_body"_FLBEXT"\n");
    return true;
}

bool emit_internal_funcs()
{
    EMIT(ERROR_LABELS_CODE);

    EMIT(EXPR_RES_BOOL_CHECK_CODE);
    EMIT(RETURN_SEMANTIC_CHECK);
    
    EMIT(OP_ASM_CODE);
    EMIT(OP_DIV_CODE);
    EMIT(OP_EQ_NEQ_CODE);

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
    if (!emit_internal_funcs()) return false;
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
    EMIT_NL("LABEL !program_body" _FLBEXT);
    //EMIT_NL("CREATEFRAME");
    return true;
}

bool emit_program_body_close()
{
    EMIT_NL("CLEARS");
    EMIT_NL("# End of program");
    return true;
}

bool emit_function_open(const char* name) 
{
    EMIT("JUMP !");
    EMIT(name);
    EMIT_NL("_skip" _FLBEXT);

    EMIT("\n# Function ");
    EMIT(name);
    EMIT("\n");

    EMIT("LABEL !");
    EMIT(name);
    EMIT(_FLBEXT "\n");

    //EMIT_NL("CREATEFRAME");
    EMIT_NL("PUSHFRAME");

    return true;
}

bool emit_function_type(DataType type, bool qmark)
{
    EMIT_NL("# Function type for semantic controls");
	EMIT_NL("DEFVAR LF@func_type");
	
	str_t str;
	str_const(&str);
	
	switch (type)
	{
		case TYPE_FLOAT	: str_concat(&str, "float")	; break;
		case TYPE_INT	: str_concat(&str, "int")  	; break;
		case TYPE_STRING: str_concat(&str, "string"); break;
		case TYPE_NULL	: str_concat(&str, "nil")	; break;
	}

	EMIT("MOVE LF@func_type "); EMIT("string@"); EMIT(str.ptr); EMIT_NL();

	str_dest(&str);

    EMIT_NL("DEFVAR LF@func_qmark");
    EMIT("MOVE LF@func_qmark "); EMIT(qmark ? "bool@true" : "bool@false"); EMIT_NL();

    return true;
}

bool emit_call_return_sem_check()
{
    EMIT_NL("PUSHS LF@func_qmark");
    EMIT_NL("PUSHS LF@func_type");
    EMIT_NL("CREATEFRAME");
	EMIT_NL("CALL !*return_semantic_check");
    return true;
}

bool emit_function_close(const char* name) 
{
    EMIT("LABEL !");
    EMIT(name);
    EMIT_NL("_end" _FLBEXT);

    EMIT_NL("POPFRAME");

    EMIT_NL("RETURN\n");

    EMIT("LABEL !");
    EMIT(name);
    EMIT_NL("_skip" _FLBEXT);

    return true;
}

bool emit_default_value(DataType type) {
    switch (type) {
        case TYPE_FLOAT:
            EMIT("float@0x0.0p+0");
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

bool emit_function_result_declaration(DataType type) {
    EMIT_NL("DEFVAR LF@res");

    EMIT("MOVE LF@res ");
    if (!emit_default_value(type)) return false;
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

bool emit_function_call(const char* name) {
    EMIT("CALL !");
    EMIT(name);
    EMIT(_FLBEXT"\n");

    return true;
}

bool emit_function_res_assign(const char* var_name, bool local_frame)
{
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
    EMIT_NL("MOVE LF@res GF@EXPR_VAL");

    EMIT("JUMP !");
    EMIT(name);
    EMIT_NL("_end" _FLBEXT);

    return true;
}

bool emit_bool_value(bool value)
{
    EMIT("bool@");
    EMIT(value ? "true" : "false");
    return true;
}

bool emit_exp_res() {
    EMIT_NL("WRITE GF@EXPR_VAL");

    return true;
}

bool emit_push(Token token, bool local_frame) {
    EMIT("PUSHS ");
    if (!emit_value_from_token(token, local_frame)) return false;
    EMIT("\n");

    return true;
}

bool emit_check_var_defined(const char* id, bool local)
{
    EMIT("TYPE GF@EXPR_TYPE "); EMIT(local ? "LF@" : "GF@"); EMIT(id); EMIT_NL();
    EMIT_NL("JUMPIFEQ !sem_error_four GF@EXPR_TYPE string@");

    return true;
}

bool emit_pop()
{
    EMIT_NL("POPS GF@TMP_REG1");
    return true;
}

bool emit_stack_operation(Rule_type rule) {
    switch (rule) 
    {
        case RULE_ADD:
            EMIT_NL("PUSHS string@add");
            EMIT_NL("CREATEFRAME");
            EMIT_NL("CALL !*op_asm");
            break;
        case RULE_SUB:
            EMIT_NL("PUSHS string@sub");
            EMIT_NL("CREATEFRAME");
            EMIT_NL("CALL !*op_asm");
            break;
        case RULE_MUL:
            EMIT_NL("PUSHS string@mul");
            EMIT_NL("CREATEFRAME");
            EMIT_NL("CALL !*op_asm");
            break;

        case RULE_EQ:
            EMIT_NL("PUSHS string@equal");
            EMIT_NL("CREATEFRAME");
            EMIT_NL("CALL !*op_eq_neq");
            break;
        case RULE_NEQ:
            EMIT_NL("PUSHS string@noteq");
            EMIT_NL("CREATEFRAME");
            EMIT_NL("CALL !*op_eq_neq");
            break;
        
        case RULE_DIV:
            EMIT_NL("CREATEFRAME");
            EMIT_NL("CALL !*op_div");
            break;

        case RULE_DOT:
            EMIT_NL("POPS GF@TMP_REG3\n"
                    "POPS GF@TMP_REG\n"
                    "CONCAT GF@TMP_REG1 GF@TMP_REG GF@TMP_REG3\n"
                    "PUSHS GF@TMP_REG1");
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
    EMIT_NL("POPS GF@TMP_REG1\n"
           "POPS GF@TMP_REG\n"
           "CONCAT GF@TMP_REG GF@TMP_REG GF@TMP_REG1\n"
           "PUSHS GF@TMP_REG");

    return true;
}

bool emit_stack_pop_res(const char* var, const char* frame) 
{
    EMIT("POPS ");
    EMIT(frame);
    EMIT("@");
    EMIT(var);
    EMIT("\n");

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
    EMIT_NL(" GF@EXPR_VAL bool@false");

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
    EMIT_NL(" GF@EXPR_VAL bool@false");

    return true;
}

bool emit_while_close(const char* name, int64_t deep, int64_t index) 
{
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

bool emit_expression_bool_convert()
{
    EMIT_NL("CREATEFRAME");
    EMIT_NL("CALL !*expr_res_bool_check\n");
    return true;
}

#if 0
bool emit_stack_top_int2float() {
    EMIT_NL("INT2FLOATS");

    return true;
}

bool emit_stack_top_float2int() {
    EMIT_NL("FLOAT2INTS");

    return true;
}

bool emit_stack_sec_int2float() {
    EMIT_NL("POPS GF@TMP_REG1\n"
           "INT2FLOATS\n"
           "PUSHS GF@TMP_REG1");

    return true;
}

bool emit_stack_sec_float2int() {
    EMIT_NL("POPS GF@TMP_REG1\n"
           "FLOAT2INTS\n"
           "PUSHS GF@TMP_REG1");

    return true;
}

bool emit_push_bool_literal(bool value)
{
    EMIT("PUSHS bool@");
    EMIT(value ? "true" : "false");
    EMIT("\n");
    return true;
}
#endif