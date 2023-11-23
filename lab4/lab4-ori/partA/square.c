#include <pthread.h>
#include <assert.h>
#include <stdio.h>

#define TODO()\
do{\
    extern int printf(char *, ...);\
    printf("Add your code here: file %s, line %d\n", __FILE__, __LINE__);\
}while(0)




void *start(void *a) {
    long n = (long)a;
    pthread_exit((void *)(n * n));
}

int main() {
    long N = 10000;
    pthread_t tids[N + 1];
    long sum = 0;
    // Exercise 1: Implement your code here
    // Add your code here:
    TODO();

    printf("sum: %ld\n", sum);  
    // 添加断言
    long expected_sum = 0;
    for (long i = 0; i <= N; ++ i) {
        expected_sum += i * i;
    }
    assert(sum == expected_sum);  // 断言sum的值等于预期值
    pthread_exit(0);
}
