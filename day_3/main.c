#include <ctype.h>
#include <file.h>
#include <hashtable.h>
#include <set.h>
#include <stdio.h>
#include <stdlib.h>
#include <str.h>
#include <string.h>

const int BUFFER_SIZE = 10;

typedef struct {
    int x;
    int y;
    char *value;
    int is_symbol;
} Token;

int append_token(Token ***tokens, const char *str, int is_symbol, int x, int y, int *total);
Token **parse_tokens(FileContents *fileContents, int *total);
Token *lookup_token(Token **tokens, int count, int x, int y);
void destroy_tokens(Token **tokens, int count);
void print_tokens(Token **tokens, int count);
void print_token(Token *token);

int get_part_number(Token *target, Token *neighbor);
int *get_part_numbers(HashTable *lookup_table, int max_x, int max_y, int *total);
int append_part_number(int **part_numbers, int part_number, int *total);
int calculate_part_numbers_sum(int *part_numbers, int count);
void print_part_numbers(int *part_numbers, int count);

HashTable *create_lookup_table(Token **tokens, int count);
void destroy_lookup_table(HashTable *lookup_table);

void print_token_value(void *value);
void print_part_number_value(void *value);

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "ERROR: Missing data filename\n");
        printf("Ussage: ./main <data_filename>\n");
        return 1;
    }

    const char *filename = argv[1];

    FileContents *file_contents = read_file_contents(filename);
    if (file_contents == NULL) {
        fprintf(stderr, "ERROR: Unable to read file %s\n", filename);
        return 2;
    }

    int max_x = strlen(file_contents->values[0]);
    int max_y = file_contents->count;

    int total_tokens;
    Token **tokens = parse_tokens(file_contents, &total_tokens);
    if (tokens == NULL) {
        fprintf(stderr, "ERROR: No tokens found\n");
        return 3;
    }

    HashTable *tokens_table = create_lookup_table(tokens, total_tokens);
    if (tokens_table == NULL) {
        fprintf(stderr, "ERROR: Unable to create tokens lookup table\n");
        return 4;
    }
    hashtable_set_print_value(tokens_table, &print_token_value);
    print_hashtable(tokens_table);

    int total_part_numbers;
    int *part_numbers = get_part_numbers(tokens_table, max_x, max_y, &total_part_numbers);
    if (part_numbers == NULL) {
        fprintf(stderr, "ERROR: No part numbers found\n");
        return 5;
    }

    print_part_numbers(part_numbers, total_part_numbers);

    int total_part_1 = calculate_part_numbers_sum(part_numbers, total_part_numbers);
    printf("part 1 | total: %d\n", total_part_1);

    free(part_numbers);
    destroy_tokens(tokens, total_tokens);
    destroy_lookup_table(tokens_table);
    destroy_file_contents(file_contents);

    return 0;
}

int append_token(Token ***tokens, const char *str, int is_symbol, int x, int y, int *total) {
    if (tokens == NULL || str == NULL || total == NULL) {
        return 0;
    }

    int count = *total;
    Token **temp = realloc(*tokens, sizeof(Token *) * count + 1);
    if (temp == NULL) {
        fprintf(stderr, "ERROR: Unable to realloc\n");
        return 0;
    }

    *tokens = temp;

    Token *token = malloc(sizeof(Token));
    token->is_symbol = is_symbol;
    token->value = strdup(str);
    token->x = x;
    token->y = y;

    (*tokens)[count] = token;
    *total = count + 1;

    return 1;
}

Token **parse_tokens(FileContents *fileContents, int *total) {
    if (fileContents == NULL || total == NULL) {
        return NULL;
    }

    *total = 0;
    int total_results = 0;
    Token **results = malloc(sizeof(Token *));

    for (int y = 0; y < fileContents->count; ++y) {
        char *str = fileContents->values[y];
        int length = strlen(str);

        int buffer_index = 0;
        int start_index = 0;
        char buffer[length];

        for (int x = 0; x < length; ++x) {
            char target = str[x];
            if (isdigit(target) != 0) {
                if (buffer_index == 0) {
                    start_index = x;
                }
                buffer[buffer_index] = target;
                buffer_index += 1;
            } else if (target == '.') {
                // If buffer has data, construct new token and reset buffer
                if (buffer_index > 0) {
                    buffer[buffer_index] = '\0';
                    if (append_token(&results, buffer, 0, start_index, y, &total_results) == 0) {
                        free(results);
                        return NULL;
                    }
                    buffer_index = 0;
                }
            } else if (target == '\n') {
                break;
            } else {
                if (buffer_index > 0) {
                    buffer[buffer_index] = '\0';
                    if (append_token(&results, buffer, 0, start_index, y, &total_results) == 0) {
                        free(results);
                        return NULL;
                    }
                    buffer_index = 0;
                }

                char symbol[] = {target, '\0'};

                if (append_token(&results, symbol, 1, x, y, &total_results) == 0) {
                    free(results);
                    return NULL;
                }
            }
        }

        if (buffer_index > 0) {
            buffer[buffer_index] = '\0';
            if (append_token(&results, buffer, 0, start_index, y, &total_results) == 0) {
                free(results);
                return NULL;
            }
        }
    }

    *total = total_results;

    return results;
}

Token *lookup_token(Token **tokens, int count, int x, int y) {
    if (tokens == NULL || x < 0 || y < 0) {
        return NULL;
    }

    for (int ty = 0; ty < count; ++ty) {
        Token *target = tokens[ty];
        if (target->x == x && target->y == y) {
            return target;
        }
    }

    return NULL;
}

void destroy_tokens(Token **tokens, int count) {
    if (tokens == NULL) {
        return;
    }

    for (int i = 0; i < count; ++i) {
        free(tokens[i]->value);
        free(tokens[i]);
    }

    free(tokens);
}

void print_tokens(Token **tokens, int count) {
    if (tokens == NULL) {
        return;
    }

    printf("total: %d\n", count);
    for (int i = 0; i < count; ++i) {
        print_token(tokens[i]);
    }
}

void print_token(Token *token) {
    if (token == NULL) {
        return;
    }
    printf("%s, x: %d, y: %d, is_symbol: %d\n", token->value, token->x, token->y, token->is_symbol);
}

int get_part_number(Token *target, Token *neighbor) {
    if (target == NULL || neighbor == NULL) {
        return -1;
    }

    if (target->is_symbol == 0 && neighbor->is_symbol == 1) {
        return atoi(target->value);
    } else if (target->is_symbol == 1 && neighbor->is_symbol == 0) {
        return atoi(neighbor->value);
    }

    return -1;
}

int *get_part_numbers(HashTable *lookup_table, int max_x, int max_y, int *total) {
    if (lookup_table == NULL) {
        return NULL;
    }

    *total = 0;

    Set *seen_values = create_set();
    if (seen_values == NULL) {
        return NULL;
    }
    set_set_print_value(seen_values, print_part_number_value);

    char key[BUFFER_SIZE];
    for (int y = 0; y < max_y; ++y) {
        for (int x = 0; x < max_x; ++x) {
            sprintf(key, "%d,%d", x, y);
            Token *target = hashtable_search(lookup_table, key);
            if (target == NULL) {
                continue;
            }

            if (target->value == NULL) {
                printf("Shouldn't happened\n");
                continue;
            }

            int lengt_offset = target->x + strlen(target->value);

            int rx = lengt_offset;
            int ry = target->y;
            sprintf(key, "%d,%d", rx, ry);
            Token *neighbor = hashtable_search(lookup_table, key);

            int part_number = get_part_number(target, neighbor);
            if (part_number > 0) {
                sprintf(key, "%d", part_number);
                set_insert(seen_values, key, (void *)(intptr_t)part_number);
            }

            int start_x = target->x - 1;
            int by = target->y + 1;

            for (int bx = start_x; bx <= lengt_offset; ++bx) {
                if (bx < 0) {
                    continue;
                }

                sprintf(key, "%d,%d", bx, by);
                neighbor = hashtable_search(lookup_table, key);
                part_number = get_part_number(target, neighbor);
                if (part_number > 0) {
                    sprintf(key, "%d", part_number);
                    set_insert(seen_values, key, (void *)(intptr_t)part_number);
                }
            }
        }
    }

    print_set(seen_values);

    void **values = set_get_values(seen_values);
    if (values == NULL) {
        return NULL;
    }

    int *results = malloc(sizeof(int) * seen_values->size);
    if (results == NULL) {
        fprintf(stderr, "ERROR: Unable to allocate memory\n");
        return NULL;
    }

    *total = seen_values->size;

    for (size_t i = 0; i < seen_values->size; ++i) {
        results[i] = ((intptr_t)values[i]);
    }

    return results;
}

int append_part_number(int **part_numbers, int part_number, int *total) {
    if (part_numbers == NULL || part_number == -1 || total == NULL) {
        return 0;
    }

    int count = *total;
    int *temp = realloc(*part_numbers, sizeof(int) * count + 1);
    if (temp == NULL) {
        fprintf(stderr, "ERROR: Unable to realloc\n");
        return 0;
    }

    *part_numbers = temp;
    (*part_numbers)[count] = part_number;
    *total = count + 1;

    return 1;
}

int calculate_part_numbers_sum(int *part_numbers, int count) {
    if (part_numbers == NULL) {
        return 0;
    }

    int total = 0;
    for (int i = 0; i < count; ++i) {
        total += part_numbers[i];
    }

    return total;
}

void print_part_numbers(int *part_numbers, int count) {
    if (part_numbers == NULL) {
        return;
    }

    printf("total: %d\n", count);
    printf("[");
    for (int i = 0; i < count; ++i) {
        printf("%d", part_numbers[i]);
        if (i != count - 1) {
            printf(", ");
        }
    }
    printf("]\n");
}

HashTable *create_lookup_table(Token **tokens, int count) {
    if (tokens == NULL) {
        return NULL;
    }

    HashTable *table = create_hashtable();
    if (table == NULL) {
        return NULL;
    }

    for (int i = 0; i < count; ++i) {
        Token *token = tokens[i];
        size_t lenght = strlen(token->value);
        char key[BUFFER_SIZE];
        if (lenght > 1) {
            size_t start = token->x;
            size_t end = start + lenght;

            for (size_t x = start; x < end; ++x) {
                sprintf(key, "%zu,%d", x, token->y);
                hashtable_insert(table, key, (void *)token);
            }

        } else {
            sprintf(key, "%d,%d", token->x, token->y);
            hashtable_insert(table, key, (void *)token);
        }
    }

    return table;
}

void destroy_lookup_table(HashTable *lookup_table) {
    if (lookup_table == NULL) {
        return;
    }
    destroy_hashtable(lookup_table);
}

void print_token_value(void *value) {
    if (value == NULL) {
        return;
    }

    Token *token = (Token *)value;
    print_token(token);
}

void print_part_number_value(void *value) {
    if (value == NULL) {
        return;
    }

    printf("%ld\n", (intptr_t)value);
}