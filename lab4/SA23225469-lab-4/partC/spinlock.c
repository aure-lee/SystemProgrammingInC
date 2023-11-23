#include <pthread.h>
#include <stdio.h>
#include <stdatomic.h>
#include <assert.h>
#include "spinlock.h"

#define TODO()\
do{\
    printf("Add your code here: file %s, line %d\n", __FILE__, __LINE__);\
}while(0)



void spinlock_init(spinlock_t *lock){
    // Exercise 2:
    // Add your code here:
    atomic_init(&lock->n, 0);
    // TODO();
}

void spinlock_lock(spinlock_t *lock){
    // Exercise 2:
    // Add your code here:
    int locked = 0;
    while (!atomic_compare_exchange_strong(&lock->n, &locked, 1)) { // atomic_compare_exchange_strong的工作原理：比较两个值并交换
        locked = 0; // 自旋锁的特点，当想要上锁时，一直判断锁的状态
    }
    // TODO();
}

void spinlock_unlock(spinlock_t *lock){
    // Exercise 2:
    // Add your code here:
    atomic_store(&lock->n, 0);
    // TODO();
}

typedef struct {
    spinlock_t lock;
    int num;
} counter_t;

counter_t counter;

void *start(){
    for(int i = 0; i < 10000; i++){
        // Exercise 2:
        // Add your code here:
        spinlock_lock(&counter.lock);
        counter.num ++;
        spinlock_unlock(&counter.lock);
        // TODO();
    }
    pthread_exit(0);
}

int main(){
    pthread_t pids[10];

    spinlock_init(&counter.lock);

    for(int i = 0; i < 10; i++){
        pthread_create(&pids[i], NULL, start, NULL);
    }

    for (int i = 0; i < 10; i++) {
        pthread_join(pids[i], NULL);
    }

    printf("Counter: %d\n", counter.num);

    assert(counter.num == 100000);
    printf("Testing spinlock: PASS");
    return 0;
}