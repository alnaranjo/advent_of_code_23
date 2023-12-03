#include <complex.h>
#include <ctype.h>
#include <iso646.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const size_t BUFFER_MAX_LENGTH = 1024;

const char *ENCODED_NUMBERS[] = {
    "zero", "one", "two",   "three", "four",
    "five", "six", "seven", "eight", "nine",
};

typedef struct {
  char **values;
  int count;
} file_contents_t;

typedef struct {
  int value;
  int index;
} found_number_item_t;

typedef struct {
  found_number_item_t *values;
  int count;
} found_numbers_t;

int compare(const void *a, const void *b);

int get_file_total_lines(FILE *fp);
int get_numbers_count(char *str);
int get_encoded_numbers_count(char *str);

file_contents_t *read_file_contents(FILE *fp);
void print_file_contents(file_contents_t *contents);
void destroy_file_contents(file_contents_t *contents);

found_numbers_t *read_numbers_in_string(char *str);
found_numbers_t *read_encoded_numbers_in_string(char *str);
void print_found_numbers(found_numbers_t *numbers);
void destroy_found_numbers(found_numbers_t *data);

int calculate_calibration_value(file_contents_t *contents);

int main() {
  FILE *fp;

  const char *filename = "input.txt";
  fp = fopen(filename, "r");

  if (fp == NULL) {
    exit(1);
  }

  file_contents_t *contents = read_file_contents(fp);
  if (!contents) {
    exit(2);
  }

  int total = calculate_calibration_value(contents);
  printf("%d", total);

  destroy_file_contents(contents);
  fclose(fp);

  return 0;
}

int compare(const void *a, const void *b) {
  found_number_item_t *val_a = (found_number_item_t *)a;
  found_number_item_t *val_b = (found_number_item_t *)b;
  return val_a->index - val_b->index;
}

int get_file_total_lines(FILE *fp) {
  if (fp == NULL) {
    return 0;
  }

  int total_lines = 0;
  char buffer[BUFFER_MAX_LENGTH];
  while (fgets(buffer, BUFFER_MAX_LENGTH, fp) != NULL) {
    total_lines += 1;
  }

  // Reset cursor
  fseek(fp, 0, SEEK_SET);

  return total_lines;
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

file_contents_t *read_file_contents(FILE *fp) {
  if (fp == NULL) {
    return NULL;
  }

  int total_lines = get_file_total_lines(fp);
  if (total_lines == 0) {
    return NULL;
  }

  int index = 0;
  char **values = malloc(sizeof(char *) * total_lines);

  char buffer[BUFFER_MAX_LENGTH];
  while (fgets(buffer, BUFFER_MAX_LENGTH, fp) != NULL) {
    int length = strlen(buffer);
    char *line = malloc(sizeof(char) * length);
    strcpy(line, buffer);

    values[index] = line;
    index += 1;
  }

  // Reset cursor
  fseek(fp, 0, SEEK_SET);

  file_contents_t *result = malloc(sizeof(file_contents_t));
  result->values = values;
  result->count = total_lines;

  return result;
}

void print_file_contents(file_contents_t *contents) {
  if (contents == NULL) {
    return;
  }

  printf("total_lines: %d\n", contents->count);

  for (size_t i = 0; i < contents->count; ++i) {
    char *str = contents->values[i];
    int length = strlen(str);
    printf("[%d] %s", length, str);
  }
}

void destroy_file_contents(file_contents_t *contents) {
  if (contents == NULL) {
    return;
  }

  if (contents->values != NULL) {
    free(contents->values);
  }

  free(contents);
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
      items[index].index = index;
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

  found_number_item_t *items =
      malloc(sizeof(found_number_item_t) * total_numbers);

  int index = 0;

  for (size_t i = 0; i < 10; ++i) {
    const char *target = ENCODED_NUMBERS[i];
    const char *found = strstr(str, target);

    while (found != NULL) {
      int offset = found - str;
      int value = i;
      items[index].index = offset;
      items[index].value = i;

      found = strstr(found + strlen(target), target);
      total_numbers += 1;
    }
  }

  found_numbers_t *result = malloc(sizeof(found_numbers_t));
  result->values = items;
  result->count = total_numbers;

  return result;
}

void print_found_numbers(found_numbers_t *numbers) {
  if (numbers == NULL) {
    return;
  }

  printf("total_numbers: %d\n", numbers->count);
  for (size_t i = 0; i < numbers->count; ++i) {
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

int calculate_calibration_value(file_contents_t *contents) {
  if (contents == NULL) {
    return 0;
  }

  int total = 0;
  found_numbers_t **numbers =
      malloc(sizeof(found_numbers_t *) * contents->count);

  for (size_t i = 0; i < contents->count; ++i) {
    found_numbers_t *found = read_numbers_in_string(contents->values[i]);

    if (found != NULL) {
      numbers[i] = found;
    }

    int first = found->values[0].value;
    int last = found->values[found->count - 1].value;
    int value = first * 10 + last;

    total += value;
  }

  return total;
}
