#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "lf-queue.h"

#define TODO()\
do{\
    printf("Add your code here: file %s, line %d\n", __FILE__, __LINE__);\
}while(0)




void lf_queue_init(lf_queue_t *queue){
    node_t *node = (node_t *)calloc(1, sizeof(*node));
    atomic_store(&queue->front, node);
    atomic_store(&queue->rear, node);
    return;
}


void lf_queue_enqueue(lf_queue_t *queue, int value){
    // Exercise 2: fill in your code:
    // Add your code here:
    node_t *node = (node_t *)malloc(sizeof(node_t));
    node->value = value;
    node->next = NULL;
    node_t *old_rear;
    do {
        old_rear = atomic_load(&queue->rear);
    } while (!atomic_compare_exchange_weak(&old_rear->next, &(node_t *){NULL}, node));

    atomic_compare_exchange_weak(&queue->rear, &old_rear, node);
    return;
}

int lf_queue_dequeue(lf_queue_t *queue){
    // Exercise 2: fill in your code:
    // Add your code here:
    node_t *old_front;
    int value;
    while (1) {
        old_front = atomic_load(&queue->front);
        if (old_front == atomic_load(&queue->rear)) {
            return -1;
        }
        node_t *next = old_front->next;
        value = next->value;
        if (atomic_compare_exchange_strong(&queue->front, &old_front, next)) {
            free(old_front);
            return value;
        }
    }
    // TODO();
    return -1;
}

int lf_queue_size(lf_queue_t *queue){
    int size = 0;
    // Exercise 2: fill in your code:
    // Add your code here:
    node_t *head = atomic_load(&queue->front)->next;
    while (head != NULL) {
        size ++;
        node_t *next = head->next;
        head = next;
    }
    // TODO();
    return size;    
}
