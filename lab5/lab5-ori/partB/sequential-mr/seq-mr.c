
#define TODO()\
do{\
    extern int printf(char *, ...);\
    printf("Add your code here: file %s, line %d\n", __FILE__, __LINE__);\
}while(0)



// seq-mr.c
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "../util/key-value.h"
#include "../lib/list.h"
#include "../lib/map-reduce.h"
#include "file-struct.h"
#include <assert.h>
#include <dlfcn.h>

const char *TMP_DIR = "/tmp/";
const char *OUTPUT_DIR = "./output/";

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

static unsigned int djb_hash(const char *cp)
{
    unsigned hash = 5381;
    while (*cp)
        hash = 33 * hash ^ (unsigned char)*cp++;
    return hash;
}

// return all charaters in a file.
char *read_file(char *file_name)
{
    char path[256];
    sprintf(path, "./../data/%s", file_name);
    FILE *fp = fopen(path, "r+");
    if (fp == NULL)
    {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    struct stat st;
    if (stat(path, &st) != 0)
    {
        perror("stat");
        exit(EXIT_FAILURE);
    }
    long file_size = st.st_size;

    char *buf = malloc(file_size + 1);
    if (buf == NULL)
    {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    fread(buf, 1, file_size, fp);
    buf[file_size] = '\0';

    fclose(fp);
    return buf;
}

void map_phase(fnode *node, int nInput, int nOutputs)
{
    // map phase
    // Exercise:
    // Add your code here:
    TODO();

}

void reduce_phase(fnode *node, int nInput, int nOutputs)
{
    // reduce phase
    // Exercise:
    // Add your code here:
    TODO();

}

int main(int argc, char *argv[])
{
     if (argc < 4)
    {
        fprintf(stderr, "Usage: seq-mr.out <nOutputs> <libmr.so> <file1> <file2> ... <fileN>\n");
        exit(0);
    }
    
    int nOutputs = atoi(argv[1]);
    const char *libmr_so = argv[2];
    load_mr_library(libmr_so);
    int nInput = argc - 3;

    fnode *node = init_fnode(nInput, (const char **)argv + 2, nOutputs, TMP_DIR, OUTPUT_DIR);

    map_phase(node, nInput, nOutputs);
    reduce_phase(node, nInput, nOutputs);

    free_fnode(node, nOutputs);

    return 0;
}