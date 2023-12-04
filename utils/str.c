#include "str.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

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