#include "symbol_stack.h"
#include "debug.h"

#define NUMFDT 6

static const char* dtstr[NUMFDT] = {
    "FLOAT",
    "INT",
    "STRING",
    "BOOL",
    "UNDEF",
    "NULL"
};

#define NUMFOPT 21
static const char* opstr[NUMFOPT] = {
    "+",       
    "-",       
    ".",       
    "*",       
    "/",       
    "===",        
    "!==",       
    "<",        
    ">",        
    "<=",       
    ">=",       
    "(",       
    ")",       

    "id",        
    "int",       
    "float",     
    "string",
    "null",   

    "$",    
    "REDUCE",   
    "E"       
};


static int s_Iter = 0;
void stack_print(SymbolStack* stack, const char* func)
{
    EXPRDBGPR("-%d :: %s\n", s_Iter, func);
    int cnt = 0;
    for(SSElement *tmp = stack->top; tmp != NULL; tmp = tmp->next, cnt++) 
    {
        EXPRDBGPR("\t%d: %-8s %s\n", cnt, opstr[tmp->operType], dtstr[tmp->dataType]);
    }
    EXPRDBGPR("\n");
    ++s_Iter;
}

void stack_init(SymbolStack* stack) 
{
    IFJ22_ASSERT(stack, "Stack is null.");
    stack->top = NULL;

    stack_print(stack, __func__);
}

bool stack_push(SymbolStack* stack, Oper_type operType, DataType dataType) 
{
    IFJ22_ASSERT(stack, "Stack is null.");
    SSElement *new = malloc(sizeof(SSElement));
    if (new == NULL)
        return false;
    new->operType = operType;
    new->dataType = dataType;
    new->next = stack->top;
    stack->top = new;

    stack_print(stack, __func__);
    return true;
}

void stack_pop(SymbolStack* stack) 
{
    IFJ22_ASSERT(stack, "Stack is null.");
    SSElement *tmp = stack->top;
    stack->top = tmp->next;
    free(tmp);
    stack_print(stack, __func__);
}

void stack_clear(SymbolStack* stack)
{
    IFJ22_ASSERT(stack, "Stack is null.");
    while(stack->top != NULL) {
        stack_pop(stack);
    }
    stack_print(stack, __func__);
}

bool stack_push_after_top_term(SymbolStack* stack, Oper_type operType, DataType dataType)
{
    SSElement* prev = NULL;

	for (SSElement* curr = stack->top; curr != NULL; curr = curr->next)
	{
		if (curr->operType < OPER_REDUCE)
		{
			SSElement* new = malloc(sizeof(SSElement));

			if (new == NULL)
				return false;

			new->operType = operType;
			new->dataType = dataType;

			if (prev == NULL)
			{
				new->next = stack->top;
				stack->top = new;
			}
			else
			{
				new->next = prev->next;
				prev->next = new;
			}

			return true;
		}

		prev = curr;
	}
    stack_print(stack, __func__);

	return false;
}

SSElement* stack_get_top_term(SymbolStack* stack) 
{
    SSElement *tmp = stack->top;
    
    for(SSElement *tmp = stack->top; tmp != NULL; tmp = tmp->next) 
    {
        if(tmp->operType < OPER_REDUCE)
            return tmp;
    }
    return NULL;
}