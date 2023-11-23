#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include "lf-stack.h"

#define TODO()\
do{\
    printf("Add your code here: file %s, line %d\n", __FILE__, __LINE__);\
}while(0)


void lf_stack_init(lf_stack_t *stack){
    atomic_store(&stack->top, 0);
    return;
}

void lf_stack_push(lf_stack_t *stack, int value){
    // Exercise 1: lock-free stack.
    // Add your code here:
    Node_t *node = (Node_t *)malloc(sizeof(Node_t));
    node->value = value;
    Node_t *old_top;
    while (1) {
        old_top = atomic_load(&stack->top);
        node->next = old_top;
        if (atomic_compare_exchange_strong(&stack->top, &old_top, node))
            break;
    }
    // TODO();
}

int lf_stack_pop(lf_stack_t *stack){
    // Exercise 1: lock-free stack
    // Add your code here:
    Node_t *old_top;
    Node_t *new_top;
    while (1) {
        old_top = atomic_load(&stack->top);
        if (old_top == NULL) {
            return -1; // 栈为空
        }
        new_top = old_top->next;
        if (atomic_compare_exchange_strong(&stack->top, &old_top, new_top))
            break;
    }
    int value = old_top->value;
    free(old_top);
    return value;
    // TODO();
}



int lf_stack_size(lf_stack_t *stack){
    // Exercise 1: lock-free stack
    // Add your code here:
    int count = 0;
    Node_t *node = atomic_load(&stack->top);
    while (node != NULL) {
        count ++;
        node = node->next;
    }
    // TODO();
    return count;
}

