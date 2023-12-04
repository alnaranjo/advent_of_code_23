#include <file.h>
#include <str.h>
#include <string.h>

typedef struct {
    int x;
    int y;
    char *value;
} DataPoint;

void print_split(char **split, int count);

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "ERROR: Missing data filename\n");
        printf("Ussage: ./main <data_filename>\n");
        return 1;
    }

    const char *filename = argv[1];

    FileContents *fileContents = read_file_contents(filename);
    if (fileContents == NULL) {
        fprintf(stderr, "ERROR: Unable to read file %s\n", filename);
        return 2;
    }

    print_file_contents(fileContents);

    for (size_t i = 0; i < fileContents->count; ++i) {
        int total_found = 0;
        char **split = split_string(fileContents->values[i], ".", &total_found);
        if (split == NULL || total_found < 0) {
            continue;
        }
        print_split(split, total_found);
    }

    destroy_file_contents(fileContents);

    return 0;
}

void print_split(char **split, int count) {
    if (split == NULL) {
        return;
    }

    printf("total: %d\n", count);
    for (size_t i = 0; i < count; ++i) {
        printf("[%zu] %s\n", strlen(split[i]), split[i]);
    }
}