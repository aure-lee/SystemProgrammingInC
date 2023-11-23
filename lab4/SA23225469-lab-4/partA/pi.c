#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define TODO()\
do{\
    printf("Add your code here: file %s, line %d\n", __FILE__, __LINE__);\
}while(0)



struct arg_t {
    float x;
    float y;
};

void *start(void *arg) {
    // Exercise 2: Implement your code here
    // Add your code here:
    struct arg_t *a = arg;
    float i = a->x;
    float j = a->y;
    if (i * i + j * j <= 1)
        pthread_exit((void *)1);
    pthread_exit((void *)0);
    // TODO();
}

int main(int argc, char **argv) {
    printf("%s ", argv[1]);
    long N1 = atoll(argv[1]);
    printf("%ld\n", N1);
    pthread_t tids[N1];

    // pi(concurrency)
    srand(time(NULL)); // 设置随机数种子
    for (long i = 0; i < N1; ++ i) {
        struct arg_t *arg = malloc(sizeof(*arg));
        int x = rand();
        int y = rand();
        arg->x = 1.0*x / RAND_MAX;
        arg->y = 1.0*y / RAND_MAX;
        // printf("%ld thread's x and y is %f, %f\n", i, arg->x, arg->y);
        pthread_create(&tids[i], 0, start, arg);
    }

    void *res;
    long M = 0;
    for (long i = 0; i < N1; ++ i) {
        pthread_join(tids[i], &res);
        M += (long)res;
    }
    printf("pai = %f\n", 4.0*M/N1);

    assert(4.0*M/N1 > 3.1 && 4.0*M/N1 < 3.2);

    pthread_exit(0);
}
