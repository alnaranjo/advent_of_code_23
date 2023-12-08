#include <ctype.h>
#include <file.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *ENCODED_NUMBERS[] = {
    "zero",
    "one",
    "two",
    "three",
    "four",
    "five",
    "six",
    "seven",
    "eight",
    "nine",
};

typedef struct {
    int value;
    int index;
} found_number_item_t;

typedef struct {
    found_number_item_t *values;
    int count;
} found_numbers_t;

int compare(const void *a, const void *b);

int get_numbers_count(char *str);
int get_encoded_numbers_count(char *str);

found_numbers_t *read_numbers_in_string(char *str);
found_numbers_t *read_encoded_numbers_in_string(char *str);
void print_found_numbers(found_numbers_t *numbers);
void destroy_found_numbers(found_numbers_t *data);

int calculate_calibration_value(FileContents *contents);
int calculate_calibration_value_v2(FileContents *contents);

int main(int argc, char **argv) {
    printf("Day 4\n");

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

    int total_part_1 = calculate_calibration_value(file_contents);
    printf("part 1 | total: %d\n", total_part_1);

    int total_part_2 = calculate_calibration_value_v2(file_contents);
    printf("part 2 | total: %d\n", total_part_2);

    destroy_file_contents(file_contents);
    return 0;
}

int compare(const void *a, const void *b) {
    found_number_item_t *val_a = (found_number_item_t *)a;
    found_number_item_t *val_b = (found_number_item_t *)b;
    return val_a->index - val_b->index;
}

int get_numbers_count(char *str) {
    int total_numbers = 0;
    if (str == NULL) {
        return 0;
    }

    size_t length = strlen(str);
    for (size_t i = 0; i < length; ++i) {
        char x = str[i];
        if (isdigit(x) != 0) {
            total_numbers += 1;
        }
    }

    return total_numbers;
}

int get_encoded_numbers_count(char *str) {
    int total_numbers = 0;
    if (str == NULL) {
        return 0;
    }

    for (size_t i = 0; i < 10; ++i) {
        const char *target = ENCODED_NUMBERS[i];
        const char *found = strstr(str, target);
        while (found != NULL) {
            total_numbers += 1;
            found = strstr(found + strlen(target), target);
        }
    }

    return total_numbers;
}

found_numbers_t *read_numbers_in_string(char *str) {
    if (str == NULL) {
        return NULL;
    }

    int total_numbers = get_numbers_count(str);
    if (total_numbers < 0) {
        return NULL;
    }

    found_number_item_t *items =
        malloc(sizeof(found_number_item_t) * total_numbers);

    int index = 0;
    size_t length = strlen(str);

    for (size_t i = 0; i < length; ++i) {
        char x = str[i];
        if (isdigit(x) != 0) {
            // Ascii offset
            int value = (int)x - '0';
            items[index].index = i;
            items[index].value = value;
            index += 1;
        }
    }

    found_numbers_t *result = malloc(sizeof(found_numbers_t));
    result->values = items;
    result->count = total_numbers;

    return result;
}

found_numbers_t *read_encoded_numbers_in_string(char *str) {
    if (str == NULL) {
        return NULL;
    }

    int total_numbers = get_encoded_numbers_count(str);
    if (total_numbers < 0) {
        return NULL;
    }

    found_number_item_t *all_items =
        malloc(sizeof(found_number_item_t) * total_numbers);

    int index = 0;
    for (size_t i = 0; i < 10; ++i) {
        const char *target = ENCODED_NUMBERS[i];
        const char *found = strstr(str, target);

        while (found != NULL) {
            int offset = found - str;
            all_items[index].index = offset;
            all_items[index].value = i;
            found = strstr(found + strlen(target), target);
            index += 1;
        }
    }

    qsort(all_items, total_numbers, sizeof(found_number_item_t), compare);

    found_numbers_t *result = malloc(sizeof(found_numbers_t));
    result->values = all_items;
    result->count = total_numbers;

    return result;
}

void print_found_numbers(found_numbers_t *numbers) {
    if (numbers == NULL) {
        return;
    }

    printf("total_numbers: %d\n", numbers->count);
    for (int i = 0; i < numbers->count; ++i) {
        found_number_item_t item = numbers->values[i];
        printf("\tindex: %d, value: %d\n", item.index, item.value);
    }
}

void destroy_found_numbers(found_numbers_t *data) {
    if (data == NULL) {
        return;
    }

    if (data->values != NULL) {
        free(data->values);
    }

    free(data);
}

int calculate_calibration_value(FileContents *contents) {
    if (contents == NULL) {
        return 0;
    }

    int total = 0;
    for (int i = 0; i < contents->count; ++i) {
        found_numbers_t *found = read_numbers_in_string(contents->values[i]);

        if (found == NULL) {
            return 0;
        }

        int first = found->values[0].value;
        int last = found->values[found->count - 1].value;
        int value = first * 10 + last;

        total += value;
    }

    return total;
}

int calculate_calibration_value_v2(FileContents *contents) {
    if (contents == NULL) {
        return 0;
    }

    int total = 0;
    for (int i = 0; i < contents->count; ++i) {
        char *str = contents->values[i];
        found_numbers_t *found = read_numbers_in_string(str);
        found_numbers_t *found_encoded =
            read_encoded_numbers_in_string(str);

        if (found == NULL && found_encoded == NULL) {
            return 0;
        }

        int first_index = INT_MAX;
        int last_index = INT_MIN;
        if (found != NULL && found->count > 0) {
            first_index = found->values[0].index;
            last_index = found->values[found->count - 1].index;
        }

        int first_index_encoded = INT_MAX;
        int last_index_encoded = INT_MIN;
        if (found_encoded != NULL && found_encoded->count > 0) {
            first_index_encoded = found_encoded->values[0].index;
            last_index_encoded =
                found_encoded->values[found_encoded->count - 1].index;
        }

        int first_value;
        int last_value;
        if (first_index < first_index_encoded) {
            first_value = found->values[0].value;
        } else {
            first_value = found_encoded->values[0].value;
        }

        if (last_index > last_index_encoded) {
            last_value = found->values[found->count - 1].value;
        } else {
            last_value = found_encoded->values[found_encoded->count - 1].value;
        }

        int value = first_value * 10 + last_value;

        total += value;
    }

    return total;
}
