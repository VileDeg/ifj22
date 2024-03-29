/**
 * Project: Implementation of imperative language IFJ22 compiler.
 * 
 * @author Aleksandr Shevchenko (xshevc01@stud.fit.vutbr.cz)
 * @author Vadim Goncearenco    (xgonce00@stud.fit.vutbr.cz)
 */

#include <stdlib.h>
#include <string.h>

#include "symtable.h"
#include "string_t.h"
#include "errors.h"
#include "macros.h"

size_t hash_function(const char *str) 
{
    uint32_t h=0;     // must have 32 bits
    const unsigned char *p;
    for(p=(const unsigned char*)str; *p!='\0'; p++)
        h = 65599*h + *p;
    return h % MAX_SYMTABLE_SIZE;
}

void symtable_init(TSymtable *st) 
{
    if (!st)
        return;

    for (int64_t i = 0; i < MAX_SYMTABLE_SIZE; i++)
        st->items[i] = NULL;
}

TItem *item_const(const char *key, bool *alloc_failed) 
{
    TItem *new_item = calloc(sizeof(TItem), 1);
    if (!new_item)
        goto mem_fail;
    if (!(new_item->key = calloc(1, strlen(key) + 1)))
        goto key_fail;
    if (!(new_item->data.id = calloc(1, strlen(key) + 1)))
        goto id_fail;
    if (!(new_item->data.param_str = calloc(sizeof(str_t), 1)))
        goto params_fail;
    if (!str_const(new_item->data.param_str))
        goto params_const_fail;
    strcpy(new_item->key, key);
    strcpy(new_item->data.id, key);
    
    new_item->data.type = TYPE_UNDEF;
    new_item->data.global = false;
    new_item->data.qmark_type = false;

    new_item->next = NULL;
    return new_item;
params_const_fail:
    free(new_item->data.param_str);
params_fail:
    free(new_item->data.id);
id_fail:
    free(new_item->key);
key_fail:
    free(new_item);
mem_fail:
    *alloc_failed = true;
    return NULL;
}

TData *symtable_add_symbol(TSymtable *st, const char *key, bool *alloc_failed) 
{
    *alloc_failed = false;
    if (!st || !key) 
    {
        *alloc_failed = true;
        return  NULL;
    }
    size_t index = hash_function(key);
    TItem *last = NULL;
    TItem *tmp = st->items[index];
    while (tmp) 
    {
        if (!strcmp(tmp->key, key))
            return NULL;                                //nothing to add

        last = tmp;
        tmp = tmp->next;
    }
    TItem *new_item = item_const(key, alloc_failed);
    if (!new_item)
        return NULL;
    if (!last)
        st->items[index] = new_item;
    else
        last->next = new_item;
    
    return &new_item->data;
}

bool symtable_add_param(TData *data, int64_t data_type, bool can_be_null) 
{
    if (!data)
        return false;

    #define NUMFTYPES 4
    static const int stype[NUMFTYPES] = { TYPE_FLOAT, TYPE_INT, TYPE_STRING, TYPE_UNDEF };
    static const int slett[NUMFTYPES] = { 'f', 'i', 's', 'u' };

    for (int i = 0; i < NUMFTYPES; ++i)
    {
        if (data_type == stype[i])
        {
            int sign = can_be_null ? LETTFLIP(slett[i]) : slett[i];
            if (!str_add_sign(data->param_str, sign))
                return false;
            return true;
        }
    }

    VILE_ASSERT(false, "Wrong type.");
    return false;
}

TData *symtable_find(TSymtable *st, const char *key) 
{
    if (!st || !key)
        return NULL;

    size_t index = hash_function(key);
    TItem *tmp = st->items[index];
    while (tmp != NULL) 
    {
        if (!strcmp(tmp->key, key))
            return &tmp->data;

        tmp = tmp->next;
    }
    return NULL;
}

bool symtable_delete_symbol(TSymtable *st, const char *key) 
{
    if (!st || !key)
        return false;

    size_t index = hash_function(key);
    TItem *last = NULL;
    TItem *tmp = st->items[index];
    while (tmp) 
    {
        if (!strcmp(key, tmp->key)) 
        {
            if (!last)
                st->items[index] = st->items[index]->next;
            else
                last->next = tmp->next;

            free(tmp->key);
            if (tmp->data.param_str) {
                str_dest(tmp->data.param_str);
                free(tmp->data.param_str);
            }
            free(tmp);
            return true;
        }
        last = tmp;
        tmp = tmp->next;
    }
    return false;
}

void symtable_dest(TSymtable *st) 
{
    if (!st)
        return;

    for (int64_t i = 0; i < MAX_SYMTABLE_SIZE; i++) 
    {
        TItem *to_delete = st->items[i];
        TItem *tmp = NULL;
        while (to_delete) 
        {
            tmp = to_delete->next;
            free(to_delete->key);
            if (to_delete->data.param_str) 
            {
                str_dest(to_delete->data.param_str);
                free(to_delete->data.param_str);
            }
            free(to_delete->data.id);
            free(to_delete);
            to_delete = tmp;
        }
        st->items[i] = NULL;
    }
}