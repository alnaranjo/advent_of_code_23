#include <ctype.h>
#include <file.h>
#include <stdlib.h>
#include <str.h>
#include <string.h>

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

    int total_tokens;
    Token **tokens = parse_tokens(file_contents, &total_tokens);
    print_tokens(tokens, total_tokens);

    destroy_tokens(tokens, total_tokens);
    destroy_file_contents(file_contents);

    return 0;
}

int append_token(Token ***tokens, const char *str, int is_symbol, int x, int y, int *total) {
    if (tokens == NULL || str == NULL || total == NULL) {
        return 1;
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
    if (tokens == NULL) {
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

SplitStr *lookup_split_str(SplitStr **split, int count, int x, int y) {
    if (split == NULL) {
        return NULL;
    }

    if (y >= 0 && y < count) {
        SplitStr *line = split[y];
        for (int i = 0; i < line->count; ++i) {
            if (line->indexes[i] == x) {
                return line;
            }
        }
    }

    return NULL;
}
