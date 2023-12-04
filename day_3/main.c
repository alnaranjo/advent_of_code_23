#include <file.h>
#include <stdlib.h>
#include <str.h>
#include <string.h>

typedef struct {
    int x;
    int y;
    char *value;
} DataPoint;

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

    SplitStr **results = malloc(sizeof(SplitStr *) * fileContents->count);
    for (int i = 0; i < fileContents->count; ++i) {
        SplitStr *split = split_str(fileContents->values[i], ".");
        if (split == NULL) {
            continue;
        }
        results[i] = split;
    }

    for (int i = 0; i < fileContents->count; ++i) {
        print_split_str(results[i]);
    }
    for (int i = 0; i < fileContents->count; ++i) {
        destroy_split_str(results[i]);
    }
    free(results);

    destroy_file_contents(fileContents);

    return 0;
}
