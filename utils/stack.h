#ifndef _STACK_H
#define _STACK_H

#include <stdlib.h>

#define STACK_SIZE 100

typedef struct {
    void *data[STACK_SIZE];
    size_t size;
    int top;
    void (*prit_value)(void *value);
} Stack;

typedef void (*stack_print_value_t)(void *value);

Stack *create_stack();
void destroy_stack(Stack *stack);
int stack_pusk(Stack *stack, void *value);
void *stack_pop(Stack *stack);
void set_print_value(Stack *stack, stack_print_value_t fp);
void print_stack(Stack *stack);

#endif