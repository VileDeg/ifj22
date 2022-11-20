#include <stdlib.h>
#include <string.h>

#include "symtable.h"
#include "string_t.h"
#include "errors.h"
#include "macros.h"


size_t hash_function(const char *str) {
    uint32_t h=0;     // must have 32 bits
    const unsigned char *p;
    for(p=(const unsigned char*)str; *p!='\0'; p++)
        h = 65599*h + *p;
    return h % MAX_SYMTABLE_SIZE;
}


void symtable_init(TSymtable *st) {
    if (st == NULL) {
        return;
    }
    for (int64_t i = 0; i < MAX_SYMTABLE_SIZE; i++) {
        st->items[i] = NULL;
    }
}


TItem *item_const(const char *key, bool *alloc_failed) 
{
    TItem *new_item = calloc(sizeof(TItem), 1);
    if (new_item == NULL)
        goto mem_fail;
    if (!(new_item->key = calloc(1, strlen(key) + 1)))
        goto key_fail;
    if (!(new_item->data.id = calloc(1, strlen(key) + 1)))
        goto id_fail;
    if (!(new_item->data.params = calloc(sizeof(str_t), 1)))
        goto params_fail;
    if (!str_const(new_item->data.params))
        goto params_const_fail;
    strcpy(new_item->key, key);
    strcpy(new_item->data.id, key);
    
    new_item->data.type = TYPE_UNDEF;
    new_item->data.global = false;
    new_item->data.qmark_type = false;

    new_item->next = NULL;
    return new_item;
params_const_fail:
    free(new_item->data.params);
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


TData *symtable_add_symbol(TSymtable *st, const char *key, bool *alloc_failed) {
    *alloc_failed = false;
    if (st == NULL || key == NULL) {
        *alloc_failed = true;
        return  NULL;
    }
    size_t index = hash_function(key);
    TItem *last = NULL;
    TItem *tmp = st->items[index];
    while (tmp != NULL) {
        if (strcmp(tmp->key, key) == 0) {
            return NULL;                                //nothing to add
        }
        last = tmp;
        tmp = tmp->next;
    }
    TItem *new_item = item_const(key, alloc_failed);
    if (new_item == NULL)
        return NULL;
    if (last == NULL) {
        st->items[index] = new_item;
    } else {
        last->next = new_item;
    }
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
            if (!str_add_sign(data->params, sign))
                return false;
            return true;
        }
    }

    IFJ22_ASSERT(false, "Wrong type.");
    return false;
}


TData *symtable_find(TSymtable *st, const char *key) {
    if (st == NULL || key == NULL) {
        return NULL;
    }
    size_t index = hash_function(key);
    TItem *tmp = st->items[index];
    while (tmp != NULL) {
        if (strcmp(tmp->key, key) == 0) {
            return &tmp->data;
        }
        tmp = tmp->next;
    }
    return NULL;
}


bool symtable_delete_symbol(TSymtable *st, const char *key) {
    if (st == NULL || key == NULL) {
        return false;
    }
    size_t index = hash_function(key);
    TItem *last = NULL;
    TItem *tmp = st->items[index];
    while (tmp != NULL) {
        if (strcmp(key, tmp->key) == 0) {
            if (last == NULL) {
                st->items[index] = st->items[index]->next;
            } else {
                last->next = tmp->next;
            }
            free(tmp->key);
            if (tmp->data.params != NULL) {
                str_dest(tmp->data.params);
                free(tmp->data.params);
            }
            free(tmp);
            return true;
        }
        last = tmp;
        tmp = tmp->next;
    }
    return false;
}


void symtable_dest(TSymtable *st) {
    if (st == NULL) {
        return;
    }

    for (int64_t i = 0; i < MAX_SYMTABLE_SIZE; i++) {
        TItem *to_delete = st->items[i];
        TItem *tmp = NULL;
        while (to_delete != NULL) {
            tmp = to_delete->next;
            free(to_delete->key);
            if (to_delete->data.params != NULL) {
                str_dest(to_delete->data.params);
                free(to_delete->data.params);
            }
            free(to_delete->data.id);
            free(to_delete);
            to_delete = tmp;
        }
        st->items[i] = NULL;
    }
}