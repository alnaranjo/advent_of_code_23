#ifndef STR_H
#define STR_H

typedef struct {
    char **values;
    int *indexes;
    int count;
} SplitStr;

char *trim_string(char *c);

char **split_string(const char *str, const char *delimit, int *total_count);
void print_split(char **split, int count);

SplitStr *split_str(const char *str, const char *delimit);
void destroy_split_str(SplitStr *split);
void print_split_str(SplitStr *split);

#endif