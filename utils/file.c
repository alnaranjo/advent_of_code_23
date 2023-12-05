#include "file.h"
#include <stdlib.h>
#include <string.h>

const size_t BUFFER_MAX_LENGTH = 1024;

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

FileContents *read_file_contents(const char *filename) {
    if (filename == NULL) {
        return NULL;
    }

    FILE *fp = fopen(filename, "r");
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
        char *line = strdup(buffer);
        values[index] = line;
        index += 1;
    }

    // Reset cursor
    fseek(fp, 0, SEEK_SET);

    FileContents *result = malloc(sizeof(FileContents));
    result->values = values;
    result->fp = fp;
    result->count = total_lines;

    return result;
}

void print_file_contents(FileContents *contents) {
    if (contents == NULL) {
        return;
    }

    printf("total_lines: %d\n", contents->count);

    for (int i = 0; i < contents->count; ++i) {
        char *str = contents->values[i];
        int length = strlen(str);
        printf("[%d] %s", length, str);
    }

    printf("\n");
}

void destroy_file_contents(FileContents *contents) {
    if (contents == NULL) {
        return;
    }

    if (contents->values != NULL) {
        free(contents->values);
    }

    if (contents->fp != NULL) {
        fclose(contents->fp);
    }

    free(contents);
}