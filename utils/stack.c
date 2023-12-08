#include "stack.h"
#include <stdio.h>
#include <stdlib.h>

Stack *create_stack() {
    Stack *stack = malloc(sizeof(Stack));
    if (stack == NULL) {
        fprintf(stderr, "ERROR: Unable to allocate memory\n");
        return NULL;
    }

    for (size_t i = 0; i < STACK_SIZE; ++i) {
        stack->data[i] = NULL;
    }
    stack->size = 0;
    stack->top = -1;

    return stack;
}

void destroy_stack(Stack *stack) {
    if (stack == NULL) {
        return;
    }
    free(stack);
    stack = NULL;
}

int stack_pusk(Stack *stack, void *value) {
    if (stack == NULL || value == NULL) {
        return 0;
    }

    if (stack->size == STACK_SIZE - 1) {
        fprintf(stderr, "ERROR: Stack full");
        return 0;
    }

    stack->data[stack->size] = value;
    stack->size += 1;
    stack->top += 1;

    return 1;
}

void *stack_pop(Stack *stack) {
    if (stack == NULL || stack->top == -1) {
        return NULL;
    }

    void *value = stack->data[stack->top];
    stack->top -= 1;

    return value;
}

void set_print_value(Stack *stack, stack_print_value_t fp) {
    if (stack == NULL || fp == NULL) {
        return;
    }
    stack->prit_value = fp;
}

void print_stack(Stack *stack) {
    if (stack == NULL) {
        return;
    }

    for (size_t i = 0; i < STACK_SIZE; ++i) {
        if (stack->prit_value != NULL) {
            (*stack->prit_value)(stack->data[i]);
        } else {
            printf("%p\n", stack->data[i]);
        }
    }
}