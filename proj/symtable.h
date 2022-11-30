#ifndef __SYMTABLE__
#define __SYMTABLE__

#include <stdio.h>

#include "string_t.h"

#define MAX_SYMTABLE_SIZE 10007

/**
 * Symbol type.
 */
typedef enum {
    TYPE_FLOAT,
    TYPE_INT,
    TYPE_STRING,

    TYPE_BOOL,  
    TYPE_UNDEF,
    TYPE_NULL
} DataType;

/**
 * Symbol data.
 */
typedef struct
{
    DataType type;
    str_t* params;
    char* id;
    bool global;
    bool qmark_type;
} TData;

/**
 * Symtable item.
 */
typedef struct item {
    struct item *next;
    TData data;
    char *key;
} TItem;

/**
 * Symbol table.
 */
typedef struct {
    TItem* items[MAX_SYMTABLE_SIZE]; //array of pointers
} TSymtable;

/**
 * Suitable hash function for strings (inspired of IJC).
 * http://www.cse.yorku.ca/~oz/hash.html
 * @param str input string.
 * @return Index to the hash table.
 */
size_t hash_function(const char *str);

/**
 * Initialization of symbol table.
 * @param st pointer to symbol table.
 */
void symtable_init(TSymtable *st);

/**
 * Constructor of symbol table item.
 * @param key key
 * @param alloc_failed error variable
 * @return Pointer to the item.
 */
TItem *item_const(const char *key, bool *alloc_failed);

/**
 * Searching for "key" in symbol table
 * If there is no match, creating new item in symbol table.
 * @param st symbol table
 * @param key key
 * @param alloc_failed error variable
 * @return Pointer to added item.
 */
TData *symtable_add_symbol(TSymtable *st, const char *key, bool *alloc_failed);

/**
 * Adding parameter to symbol.
 * @param data symbol data
 * @param data_type symbol type
 * @return True if success.
 */
bool symtable_add_param(TData *data, int64_t data_type, bool can_be_null);

/**
 * Searching for "key" in symtable.
 * @param st symbol table
 * @param key key
 * @return Data of symbol.
 */
TData *symtable_find(TSymtable *st, const char *key);

/**
 * Delete "key" from the symbol table.
 * @param st symbol table
 * @param key key
 * @return True if success.
 */
bool symtable_delete_symbol(TSymtable *st, const char *key);

/**
 * Delete all items in symbol table.
 * @param st symbol table
 */
void symtable_dest(TSymtable *st);

#endif //__SYMTABLE__