#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <pthread.h>
#include <dlfcn.h>
#include "../util/key-value.h"
#include "../lib/list.h"
#include "../lib/map-reduce.h"
#include "mr.h"
#include "mr-rpcs.h"

#define TODO()\
do{\
    extern int printf(char *, ...);\
    printf("Add your code here: file %s, line %d\n", __FILE__, __LINE__);\
}while(0)




list_t (*mr_map)(char *);
char *(*mr_reduce)(char *, list_t);

void load_mr_library(const char *libmr_so){
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

int main(int argc, char *argv[]){
    if (argc < 2)
    {
        fprintf(stderr, "Usage: ./mr-client.out <libmr.so>\n");
        exit(0);
    }
    const char *libmr_so = argv[1];
    load_mr_library(libmr_so);
    // Exercise:
    // Add your code here:
    TODO();

}