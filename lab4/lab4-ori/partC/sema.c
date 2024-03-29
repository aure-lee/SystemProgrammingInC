#include <pthread.h>
#include <stdio.h>
#include <stdatomic.h>
#include <assert.h>
#include "sema.h"

#define TODO()\
do{\
    extern int printf(char *, ...);\
    printf("Add your code here: file %s, line %d\n", __FILE__, __LINE__);\
}while(0)




void sema_init(sema_t *sem, int v) {
    // Exercise 2:
    // Add your code here:
    TODO();

}

void sema_wait(sema_t *sem) {
    // Exercise 2:
    // Add your code here:
    TODO();

}

void sema_post(sema_t *sem) {
    // Exercise 2:
    // Add your code here:
    TODO();

}

typedef struct {
    sema_t sem;
    int num;
} counter_t;

counter_t counter;

void *start(void *arg) {
    for (int i = 0; i < 10000; i++) {
        // Exercise 2:
        // Add your code here:
        TODO();

    }
    pthread_exit(0);
}

int main() {
    pthread_t pids[10];
    sema_init(&counter.sem, 1);

    for (int i = 0; i < 10; i++) {
        pthread_create(&pids[i], NULL, start, NULL);
    }

    for (int i = 0; i < 10; i++) {
        pthread_join(pids[i], NULL);
    }

    printf("Counter: %d\n", counter.num);
    assert(counter.num == 100000);
    return 0;
}