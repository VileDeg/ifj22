#include "errors.h"

const char* g_ErrStr[NUMFERRORS] = {
    "SUCCESS",
    "ERROR_LEXICAL",
    "ERROR_SYNTAX",
    "ERROR_SEM_ID_DEF",
    "ERROR_SEM_TYPE_COMPAT",
    "ERROR_SEM_UNDEF_VAR",
    "ERROR_SEM_RETURN",
    "ERROR_SEM_EXPRESSION",
    "ERROR_SEM_OTHER",
};