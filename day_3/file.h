#ifndef FILE_H
#define FILE_H

#include <stdio.h>

typedef struct {
  char **values;
  FILE *fp;
  int count;
} FileContents;


int get_file_total_lines(FILE *fp);
FileContents *read_file_contents(const char *filename);
void print_file_contents(FileContents *contents);
void destroy_file_contents(FileContents *contents);

#endif