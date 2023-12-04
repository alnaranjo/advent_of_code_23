#include "file.h"

const char *DATA_FILENAME = "test.txt";

int main(void) {
  FileContents *fileContents = read_file_contents(DATA_FILENAME);
  print_file_contents(fileContents);
  return 0;
}
