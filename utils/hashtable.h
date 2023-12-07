#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <stdlib.h>

#define HASHTABLE_SIZE 100

typedef struct HashNode {
    char *key;
    void *value;
    struct HashNode *next;
} HashNode;

typedef void (*hashtable_print_value_t)(void *value);

typedef struct {
    HashNode *buckets[HASHTABLE_SIZE];
    size_t size;
    void (*print_value)(void *value);
} HashTable;

HashTable *create_hashtable();
void destroy_hashtable(HashTable *table);

HashNode *create_hashnode(const char *key, void *value);
void destroy_hashnode(HashNode *node);

void hashtable_set_print_value(HashTable *table, hashtable_print_value_t fp);
void hashtable_insert(HashTable *table, const char *key, void *value);
void *hashtable_search(const HashTable *table, const char *key);
void hashtable_delete(HashTable *table, const char *key);
void hashtable_cleanup(HashTable *table);
void print_hashtable(HashTable *table);

#endif

// 0x000000011f704440 "467"