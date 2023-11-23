#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "concur-stack.h"

#define TODO()\
do{\
    printf("Add your code here: file %s, line %d\n", __FILE__, __LINE__);\
}while(0)


void concur_stack_init(concur_stack_t *stack){
    pthread_mutex_init(&stack->mu, NULL);
    return;
}

void concur_stack_push(concur_stack_t *stack, int value){
    // Exercise 1: complete other operations of the concurrent stack.
    // Add your code here:
    struct Node_t *node = malloc(sizeof(struct Node_t));
    node->value = value;
    pthread_mutex_lock(&stack->mu);
    node->next = stack->top;
    stack->top = node;
    pthread_mutex_unlock(&stack->mu);
    // TODO();
}

int concur_stack_pop(concur_stack_t *stack){
    int value = -1;
    // Exercise 1: complete other operations of the concurrent stack.
    // Add your code here:
    pthread_mutex_lock(&stack->mu);
    if (stack->top == NULL) {
        pthread_mutex_unlock(&stack->mu);
        return value;
    }
    struct Node_t *top = stack->top;
    value = top->value;
    stack->top = top->next;
    pthread_mutex_unlock(&stack->mu);
    // TODO();
    free(top);
    return value;
}

int concur_stack_size(concur_stack_t *stack){
    int size = 0;
    // Exercise 1: complete other operations of the concurrent stack.
    // Add your code here:
    pthread_mutex_lock(&stack->mu);

    struct Node_t *p = stack->top;
    while (p != NULL) {
        size ++;
        p = p->next;
    }
    pthread_mutex_unlock(&stack->mu);
    // TODO();
    return size;
}
