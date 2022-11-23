#ifndef __SYMBOL_STACK__
#define __SYMBOL_STACK__

#include "expression.h"
#include "symtable.h"

/*
 * @struct Stack element structure.
 */
typedef struct symbol {
    Oper_type operType;
    struct symbol *next;
    DataType dataType;
} Symbol;

/*
 * @struct Stack structure.
 */
typedef struct {
    Symbol *top;
} SymbolStack;

void stack_init(SymbolStack* stack);

bool stack_push(SymbolStack* stack, Oper_type operType, DataType dataType);

void stack_pop(SymbolStack* stack);

void stack_pop_count(SymbolStack* stack, int cnt);

void stack_clear(SymbolStack* stack);

Symbol* stack_get_top_term(SymbolStack* stack);

bool stack_push_after_top_term(SymbolStack* stack, Oper_type operType, DataType dataType);

#endif // __SYMBOL_STACK__