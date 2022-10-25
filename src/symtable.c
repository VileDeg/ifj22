#include "symtable.h"
#include "string_t.h"
#include <stdlib.h>
#include <string.h>


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


TItem *item_const(const char *key, bool *alloc_failed) {
    TItem *new_item = (TItem *) calloc(sizeof(TItem), 1);
    if (new_item == NULL) {
        *alloc_failed = true;
        return NULL;
    }
    new_item->key = (char *) calloc(1, strlen(key) + 1);
    if (new_item->key == NULL) {
        free(new_item);
        *alloc_failed = true;
        return NULL;
    }
    new_item->data.string = (str_t *) calloc(sizeof(str_t), 1);
    if (new_item->data.string == NULL) {
        free(new_item->key);
        free(new_item);
        *alloc_failed = true;
        return NULL;
    }
    if (!str_const(new_item->data.string)) {
        free(new_item->key);
        free(new_item->data.string);
        free(new_item);
        *alloc_failed = true;
        return NULL;
    }
    strcpy(new_item->key, key);
    new_item->next = NULL;
    return new_item;
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


bool symtable_add_param(TData *data, int64_t data_type) {
    if (data == NULL) {
        return false;
    }
    switch (data_type) {
        case (TYPE_FLOAT):
            if (!str_add_sign(data->string, 'f')) {
                return false;
            }
            break;

        case (TYPE_INT):
            if (!str_add_sign(data->string, 'i')) {
                return false;
            }
            break;

        case (TYPE_STRING):
            if (!str_add_sign(data->string, 's')) {
                return false;
            }
            break;
        default:
            break;
    }
    return true;
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
            if (tmp->data.string != NULL) {
                str_dest(tmp->data.string);
                free(tmp->data.string);
            }
            free(tmp);
            return true;
        }
        last = tmp;
        tmp = tmp->next;
    }
    return false;
}


void symtable_clear(TSymtable *st) {
    if (st == NULL) {
        return;
    }

    for (int64_t i = 0; i < MAX_SYMTABLE_SIZE; i++) {
        TItem *to_delete = st->items[i];
        TItem *tmp = NULL;
        while (to_delete != NULL) {
            tmp = to_delete->next;
            free(to_delete->key);
            if (to_delete->data.string != NULL) {
                str_dest(to_delete->data.string);
                free(to_delete->data.string);
            }
            free(to_delete);
            to_delete = tmp;
        }
        st->items[i] = NULL;
    }
}