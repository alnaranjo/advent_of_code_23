#include "set.h"
#include <stdio.h>
#include <string.h>

size_t hash_set_key(const char *key) {
    if (key == NULL) {
        return -1;
    }

    size_t hash = 0;
    while (*key) {
        hash = (hash * 31) + *key;
        ++key;
    }

    return hash % SET_SIZE;
}

Set *create_set() {
    Set *set = malloc(sizeof(Set));

    if (set == NULL) {
        fprintf(stderr, "ERROR: Unable to allocate memory\n");
        return NULL;
    }

    for (size_t i = 0; i < SET_SIZE; ++i) {
        set->buckets[i] = NULL;
    }
    set->size = 0;

    return set;
}

void destroy_set(Set *set) {
    if (set == NULL) {
        return;
    }
    set_cleanup(set);
    free(set);
    set = NULL;
}

SetNode *create_setnode(const char *key, void *value) {
    SetNode *node = malloc(sizeof(SetNode));
    if (node == NULL) {
        fprintf(stderr, "ERROR: Unable to allocate memory");
        return NULL;
    }

    node->key = strdup(key);
    node->value = value;
    node->next = NULL;

    return node;
}

void destroy_setnode(SetNode *node) {
    if (node == NULL) {
        return;
    }

    free(node->key);
    free(node);
    node = NULL;
}

void set_set_print_value(Set *set, set_print_value_t fp) {
    if (set == NULL || fp == NULL) {
        return;
    }
    set->print_value = fp;
}

void set_insert(Set *set, const char *key, void *value) {
    if (set == NULL || key == NULL) {
        return;
    }

    if (set_contains(set, key) != 0) {
        return;
    }

    size_t index = hash_set_key(key);

    SetNode *new_node = create_setnode(key, value);
    new_node->next = set->buckets[index];
    set->buckets[index] = new_node;
    set->size += 1;
}

void *set_search(const Set *set, const char *key) {
    if (set == NULL || key == NULL) {
        return NULL;
    }

    size_t index = hash_set_key(key);

    SetNode *current = set->buckets[index];
    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            return current->value;
        }
        current = current->next;
    }

    return NULL;
}

int set_contains(Set *set, const char *key) {
    if (set == NULL || key == NULL) {
        return 0;
    }

    size_t index = hash_set_key(key);

    SetNode *current = set->buckets[index];
    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            return 1;
        }
        current = current->next;
    }

    return 0;
}

char **set_get_keys(Set *set) {
    if (set == NULL) {
        return NULL;
    }

    char **results = malloc(sizeof(char *) * set->size);
    if (results == NULL) {
        fprintf(stderr, "ERROR: Unable to allocate memory\n");
        return NULL;
    }

    size_t index = 0;
    for (size_t i = 0; i < SET_SIZE; ++i) {
        SetNode *current = set->buckets[i];
        while (current != NULL) {
            results[index] = strdup(current->key);
            current = current->next;
            index += 1;
        }
    }

    return results;
}

void **set_get_values(Set *set) {
    if (set == NULL) {
        return NULL;
    }

    void **results = malloc(sizeof(void *) * set->size);
    if (results == NULL) {
        fprintf(stderr, "ERROR: Unable to allocate memory\n");
        return NULL;
    }

    size_t index = 0;
    for (size_t i = 0; i < SET_SIZE; ++i) {
        SetNode *current = set->buckets[i];
        while (current != NULL) {
            results[index] = current->value;
            current = current->next;
            index += 1;
        }
    }

    return results;
}

void set_delete(Set *set, const char *key) {
    if (set == NULL || key == NULL) {
        return;
    }

    size_t index = hash_set_key(key);

    SetNode *prev = NULL;
    SetNode *current = set->buckets[index];

    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            if (prev == NULL) {
                set->buckets[index] = current->next;
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

void set_cleanup(Set *set) {
    if (set == NULL) {
        return;
    }

    for (size_t i = 0; i < SET_SIZE; ++i) {
        SetNode *current = set->buckets[i];
        while (current != NULL) {
            SetNode *next = current->next;
            destroy_setnode(current);
            current = next;
        }
        set->buckets[i] = NULL;
    }
}

void print_set(Set *set) {
    if (set == NULL) {
        return;
    }

    for (size_t i = 0; i < SET_SIZE; ++i) {
        SetNode *current = set->buckets[i];
        while (current != NULL) {
            if (set->print_value != NULL) {
                (*set->print_value)(current->value);
            } else {
                printf("[%s : %p]\n", current->key, current->value);
            }
            current = current->next;
        }
    }
}
