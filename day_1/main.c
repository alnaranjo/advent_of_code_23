#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

const size_t BUFFER_MAX_LENGTH = 1024;

typedef struct {
  int index;
  int value;
} found_number_t;

const char* ENCODED_NUMBERS[] = {
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

int get_numbers_count(char *str);
void read_numbers_in_string(char *str, int* values);
int get_encoded_numbers_count(char *str);
void read_encoded_numbers_in_string(char* str, int* values, int count);
int compare( const void* a, const void* b);
int calculate_calibration_values(FILE *fp);
int calculate_encoded_calibration_values(FILE *fp);

int main() {
  FILE *fp;

  const char* filename = "input.txt";
  fp = fopen(filename, "r");

  if (fp == NULL) {
    exit(1);
  }

  int total = calculate_calibration_values(fp);
  int total_encoded = calculate_encoded_calibration_values(fp);
  printf("total: %d\n", total);
  printf("total_encoded: %d\n", total_encoded);

  fclose(fp);

  return 0;
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

void read_numbers_in_string(char *str, int* values) {
  if (str == NULL || values == 0) {
    return;
  }

  int index = 0;
  size_t length = strlen(str);

  for (size_t i = 0; i < length; ++i) {
    char x = str[i];
    if (isdigit(x) != 0) {
      // Ascii offset
      int value = (int)x - '0';
      values[index] = value;
      index += 1;
    }
  }
}

int get_encoded_numbers_count(char *str) {
  int total_numbers = 0;
  if (str == NULL) {
    return 0;
  }

  for (size_t i = 0; i < 10; ++i) {
    const char *target = ENCODED_NUMBERS[i];
    if (strstr(str, target) != NULL) {
      total_numbers += 1;
    }
  }

  printf("total_numbers: %d, %s", total_numbers, str);
  return total_numbers;
}

void read_encoded_numbers_in_string(char* str, int* values, int count) {
  if (str == NULL || values == 0) {
    return;
  }

  int index = 0;
  found_number_t found_numbers[count];

  for (size_t i = 0; i < 10; ++i) {
    const char *target = ENCODED_NUMBERS[i];
    char *found = strstr(str, target);
    if (found == NULL) {
      continue;
    }

    int index = str - found;
    int value = i;
    found_numbers[index].index = index;
    found_numbers[index].value = i;
    index += 1;
  }

  qsort(found_numbers, count, sizeof(int), compare);

  for (size_t i = 0; i < count; i++) {
    values[i] = found_numbers[i].value;
  }
}

int compare( const void* a, const void* b) {
  found_number_t val_a = *(found_number_t *)a;
  found_number_t val_b = *(found_number_t *)b;
  
  return val_a.index - val_b.index;
}

int calculate_calibration_values(FILE *fp) {
  if (fp == NULL) {
    return 0;
  }

  int total = 0;
  char buffer[BUFFER_MAX_LENGTH];

  while (fgets(buffer, BUFFER_MAX_LENGTH, fp) != NULL) {
    int total_numbers_in_line = get_numbers_count(buffer);
    int values[total_numbers_in_line];
    read_numbers_in_string(buffer, values);

    int first = values[0];
    int last = values[total_numbers_in_line - 1];

    int value = first * 10 + last;
    total += value;
  } 

  // Reset cursor
  fseek(fp, 0, SEEK_SET);

  return total;
}

int calculate_encoded_calibration_values(FILE *fp) {
  if (fp == NULL) {
    return 0;
  }

  int total = 0;
  char buffer[BUFFER_MAX_LENGTH];

  while (fgets(buffer, BUFFER_MAX_LENGTH, fp) != NULL) {
    int total_numbers_in_line = get_encoded_numbers_count(buffer);
    int values[total_numbers_in_line];
    read_encoded_numbers_in_string(buffer, values, total_numbers_in_line);

    int first = values[0];
    int last = values[total_numbers_in_line - 1];

    int value = first * 10 + last;
    total += value;
  } 

  // Reset cursor
  fseek(fp, 0, SEEK_SET);

  return total;
}