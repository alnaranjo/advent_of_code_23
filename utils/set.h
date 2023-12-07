#ifndef SET_H
#define SET_H

#include <stdlib.h>

#define SET_SIZE 100

typedef struct SetNode {
    char *key;
    void *value;
    struct SetNode *next;
} SetNode;

typedef void (*set_print_value_t)(void *value);

typedef struct {
    SetNode *buckets[SET_SIZE];
    size_t size;
    void (*print_value)(void *value);
} Set;

Set *create_set();
void destroy_set(Set *set);

SetNode *create_setnode(const char *key, void *value);
void destroy_setnode(SetNode *node);

void set_set_print_value(Set *set, set_print_value_t fp);
void set_insert(Set *set, const char *key, void *value);
void *set_search(const Set *set, const char *key);
int set_contains(Set *set, const char *key);
char **set_get_keys(Set *set);
void **set_get_values(Set *set);
void set_delete(Set *set, const char *key);
void set_cleanup(Set *set);
void print_set(Set *set);

#endif