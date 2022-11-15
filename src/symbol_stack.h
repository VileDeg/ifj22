#ifndef __SYMBOL_STACK__
#define __SYMBOL_STACK__

#include "precedence_t.h"
#include "symtable.h"

/*
 * @struct Stack element structure.
 */
typedef struct ssElement {
    Oper_type operType;
    struct ssElement *next;
    DataType dataType;
} SSElement;

/*
 * @struct Stack structure.
 */
typedef struct {
    SSElement *top;
} SymbolStack;

void stack_init(SymbolStack* stack);

bool stack_push(SymbolStack* stack, Oper_type operType, DataType dataType);

void stack_pop(SymbolStack* stack);

void stack_pop_count(SymbolStack* stack, int cnt);

void stack_clear(SymbolStack* stack);

SSElement* stack_get_top_term(SymbolStack* stack);

bool stack_push_after_top_term(SymbolStack* stack, Oper_type operType, DataType dataType);

#endif // __SYMBOL_STACK__