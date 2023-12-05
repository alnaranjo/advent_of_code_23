#include "str.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *trim_string(char *c) {
    if (c == NULL) {
        return NULL;
    }

    char *e = c + strlen(c) - 1;
    while (*c && isspace(*c))
        c++;
    while (e > c && isspace(*e))
        *e-- = '\0';
    return c;
}

char **split_string(const char *str, const char *delimit, int *total_count) {
    if (str == NULL || delimit == NULL || total_count == NULL) {
        return NULL;
    }

    char *input = strdup(str);

    int total_tokens = 0;
    char **result = malloc(sizeof(char *));
    if (result == NULL) {
        return NULL;
    }

    char *token = trim_string(strtok(input, delimit));
    while (token != NULL) {
        if (strcmp(token, "") == 0) {
            token = (strtok(NULL, delimit));
            continue;
        }

        // Make space for new tokenized values
        char **temp = realloc(result, sizeof(char *) * total_tokens + 1);
        if (temp == NULL) {
            free(result);
            return NULL;
        }
        result = temp;

        total_tokens += 1;
        result[total_tokens - 1] = token;

        token = trim_string(strtok(NULL, delimit));
    }

    if (total_tokens > 0) {
        result[total_tokens] = NULL;
        *total_count = total_tokens;
    }

    return result;
}

void print_split(char **split, int count) {
    if (split == NULL) {
        return;
    }

    printf("total: %d\n", count);
    for (int i = 0; i < count; ++i) {
        printf("[%zu] %s\n", strlen(split[i]), split[i]);
    }
}

SplitStr *split_str(const char *str, const char *delimit) {
    if (str == NULL) {
        return NULL;
    }

    char *input = strdup(str);

    int total_tokens = 0;
    char **values = malloc(sizeof(char *));
    int *indexes = malloc(sizeof(int));

    if (values == NULL || indexes == NULL) {
        fprintf(stderr, "ERROR: Unable to malloc!");
        return NULL;
    }

    char *token = trim_string(strtok(input, delimit));
    while (token != NULL) {
        if (strcmp(token, "") == 0) {
            token = trim_string(strtok(NULL, delimit));
            continue;
        }

        // Make space for new tokenized values
        char **temp_values = realloc(values, sizeof(char *) * total_tokens + 1);
        int *temp_indexes = realloc(indexes, sizeof(int) * total_tokens + 1);
        if (temp_values == NULL || temp_indexes == NULL) {
            fprintf(stderr, "ERROR: Unable to realloc!");
            free(temp_values);
            free(temp_indexes);
            return NULL;
        }
        values = temp_values;
        indexes = temp_indexes;

        int index = token - input;
        values[total_tokens] = token;
        indexes[total_tokens] = index;
        total_tokens += 1;

        token = trim_string(strtok(NULL, delimit));
    }

    SplitStr *new = malloc(sizeof(SplitStr));
    new->values = values;
    new->indexes = indexes;
    new->count = total_tokens;

    return new;
}

void destroy_split_str(SplitStr *split) {
    if (split == NULL) {
        return;
    }

    free(split->values);
    free(split->indexes);
    free(split);
}

void print_split_str(SplitStr *split) {
    if (split == NULL) {
        return;
    }

    printf("total: %d\n", split->count);
    for (int i = 0; i < split->count; ++i) {
        printf("[%d] %s\n", split->indexes[i], split->values[i]);
    }
}