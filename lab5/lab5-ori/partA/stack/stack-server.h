#ifndef STACK_H
#define STACK_H

#define TODO()\
do{\
    extern int printf(char *, ...);\
    printf("Add your code here: file %s, line %d\n", __FILE__, __LINE__);\
}while(0)




#include <pthread.h>

typedef struct Node_t{
    int value;
    struct Node_t *next;
} node_t;

typedef struct Stack_t{
// Exercise: your code here
// Add your code here:
TODO();

} stack_t;

void stack_init();
void stack_push(int value);
int stack_pop();
int stack_size();

#endif
