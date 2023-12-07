#include "hashtable.h"
#include <stdio.h>
#include <string.h>

size_t hash_map_key(const char *key) {
    if (key == NULL) {
        return -1;
    }

    size_t hash = 0;
    while (*key) {
        hash = (hash * 31) + *key;
        ++key;
    }

    return hash % HASHTABLE_SIZE;
}

HashTable *create_hashtable() {
    HashTable *table = malloc(sizeof(HashTable));

    if (table == NULL) {
        fprintf(stderr, "ERROR: Unable to allocate memory\n");
        return NULL;
    }

    for (size_t i = 0; i < HASHTABLE_SIZE; ++i) {
        table->buckets[i] = NULL;
    }
    table->size = 0;

    return table;
}

void destroy_hashtable(HashTable *table) {
    if (table == NULL) {
        return;
    }
    hashtable_cleanup(table);
    free(table);
    table = NULL;
}

HashNode *create_hashnode(const char *key, void *value) {
    HashNode *node = malloc(sizeof(HashNode));
    if (node == NULL) {
        fprintf(stderr, "ERROR: Unable to allocate memory\n");
        return NULL;
    }

    node->key = strdup(key);
    node->value = value;
    node->next = NULL;

    return node;
}

void destroy_hashnode(HashNode *node) {
    if (node == NULL) {
        return;
    }

    free(node->key);
    free(node);
    node = NULL;
}

void hashtable_set_print_value(HashTable *table, hashtable_print_value_t fp) {
    if (table == NULL || fp == NULL) {
        return;
    }
    table->print_value = fp;
}

void hashtable_insert(HashTable *table, const char *key, void *value) {
    if (table == NULL || key == NULL) {
        return;
    }

    size_t index = hash_map_key(key);
    HashNode *current = table->buckets[index];

    // Check if key exists, if so update and bail
    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            current->value = value;
            return;
        }
        current = current->next;
    }

    // Key hasn't been registered yet
    // Create new node and insert at the beginning
    HashNode *new_node = create_hashnode(key, value);
    new_node->next = table->buckets[index];
    table->buckets[index] = new_node;
    table->size += 1;
}

void *hashtable_search(const HashTable *table, const char *key) {
    if (table == NULL || key == NULL) {
        return NULL;
    }

    size_t index = hash_map_key(key);

    HashNode *current = table->buckets[index];
    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            return current->value;
        }
        current = current->next;
    }

    return NULL;
}

void hashtable_delete(HashTable *table, const char *key) {
    if (table == NULL || key == NULL) {
        return;
    }

    size_t index = hash_map_key(key);

    HashNode *prev = NULL;
    HashNode *current = table->buckets[index];

    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            if (prev == NULL) {
                table->buckets[index] = current->next;
            } else {
                prev->next = current->next;
            }

            free(current->key);
            free(current);
            return;
        }
        prev = current;
        current = current->next;
    }
}

void hashtable_cleanup(HashTable *table) {
    if (table == NULL) {
        return;
    }

    for (size_t i = 0; i < HASHTABLE_SIZE; ++i) {
        HashNode *current = table->buckets[i];
        while (current != NULL) {
            HashNode *next = current->next;
            destroy_hashnode(current);
            current = next;
        }
        table->buckets[i] = NULL;
    }
}

void print_hashtable(HashTable *table) {
    if (table == NULL) {
        return;
    }

    for (size_t i = 0; i < HASHTABLE_SIZE; ++i) {
        HashNode *current = table->buckets[i];
        while (current != NULL) {
            if (table->print_value != NULL) {
                (*table->print_value)(current->value);
            } else {
                printf("[%s : %p]\n", current->key, current->value);
            }
            current = current->next;
        }
    }
}