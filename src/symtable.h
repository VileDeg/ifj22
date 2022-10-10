#ifndef __SYMTABLE__
#define __SYMTABLE__

#include <stdio.h>
#include "string_t.h"

#define MAX_SYMTABLE_SIZE 10007

/**
 * @enum Data types.
 */
typedef enum {
    DATA_FLOAT,
    DATA_INT,
    DATA_STRING,
} Data_type;

/**
 * @struct Data.
 */
typedef struct
{
    Data_type type;
    str_t* string;
} TData;

/**
 * @struct Item structure.
 */
struct item {
    struct item *next;
    TData data;
    char *key;
};

// Definition of item structure
typedef struct item TItem;

/**
 * @struct Symbol table.
 */
typedef struct {
    TItem* items[MAX_SYMTABLE_SIZE]; //array of pointers
} TSymtable;

/**
 * Suitable hash function for strings (inspired of IJC).
 * http://www.cse.yorku.ca/~oz/hash.html
 * @param str Input string.
 * @return index to the hash table.
 */
size_t hash_function(const char *str);

/**
 * Initialization of symbol table.
 * @param st Pointer to symbol table.
 */
void symtable_init(TSymtable *st);

/**
 * Constructor of symbol table item.
 * @param key
 * @param alloc_failed
 * @return Pointer to the item.
 */
TItem *item_const(const char *key, bool *alloc_failed);

/**
 * Searching for "key" in symbol table
 * If there is no match, creating new item in symbol table.
 * @param st
 * @param key
 * @param alloc_failed
 * @return Pointer to added item.
 */
TData *symtable_add_symbol(TSymtable *st, const char *key, bool *alloc_failed);

/**
 * Adding parameter to symbol.
 * @param data
 * @param data_type
 * @return True if success.
 */
bool symtable_add_param(TData *data, int data_type);

/**
 * Searching for "key" in symtable.
 * @param t
 * @param key
 * @return Data of symbol.
 */
TData *symtable_find(TSymtable *st, const char *key);

/**
 * Delete "key" from the symbol table.
 * @param st
 * @param key
 * @return True if success.
 */
bool symtable_delete_symbol(TSymtable *st, const char *key);

/**
 * Delete all items in symbol table.
 * @param st
 */
void symtable_clear(TSymtable *st);

/**
 * Free allocated symbol table.
 * @param st
 */
void symtable_free(TSymtable *st);

#endif //__SYMTABLE__