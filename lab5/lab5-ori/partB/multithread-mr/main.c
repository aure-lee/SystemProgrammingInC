#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dlfcn.h>
#include "task.h"
#include "../util/key-value.h"
#include "../lib/list.h"
#include "../lib/map-reduce.h"

#define TODO()\
do{\
    extern int printf(char *, ...);\
    printf("Add your code here: file %s, line %d\n", __FILE__, __LINE__);\
}while(0)




#define BUFF_SIZE 1024

task_t task;  //task_t design in task.h

list_t (*mr_map)(char *);
char *(*mr_reduce)(char *, list_t);

void load_mr_library(const char *libmr_so)
{
    void *handle = dlopen(libmr_so, RTLD_LAZY);
    if (!handle) {
        fprintf(stderr, "Cannot open library: %s\n", dlerror());
        exit(1);
    }

    mr_map = dlsym(handle, "mr_map");
    if (!mr_map) {
        fprintf(stderr, "Cannot load symbol mr_map: %s\n", dlerror());
        exit(1);
    }

    mr_reduce = dlsym(handle, "mr_reduce");
    if (!mr_reduce) {
        fprintf(stderr, "Cannot load symbol mr_reduce: %s\n", dlerror());
        exit(1);
    }
}

void *work(void *arg){
    // Exercise:
    // Add your code here:
    TODO();

    pthread_exit(0);
}

int main(int argc, char *argv[]){

    if (argc < 5)
    {
        fprintf(stderr, "Usage: mthd-mr.out <libmr.so> <thread_size> <result_size> <file1> <file2> ... <fileN>\n");
        exit(0);
    }
    const char *libmr_so = argv[1];
    load_mr_library(libmr_so);
    int worker_size = atoi(argv[2]);
    int result_size = atoi(argv[3]);
    int task_size = argc - 4;
    char **file_name = (char **)calloc(task_size, sizeof(char *));

    for(int i=4; i<argc; i++)
        file_name[i-4] = argv[i];

    system("rm -rf ./output && mkdir output");
    system("rm -rf /tmp/mth_data && mkdir /tmp/mth_data");
    chmod("/tmp/mthdata", 0777);
    Task_init(&task, task_size, file_name, result_size);
    pthread_t threads[BUFF_SIZE];
    for(int i=0; i<worker_size; i++)
        pthread_create(&threads[i], NULL, work, (void*)0);

    for(int i=0; i<worker_size; i++)
        pthread_join(threads[i], NULL);
    
    return 0;
}