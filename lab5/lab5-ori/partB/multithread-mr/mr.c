#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "../util/key-value.h"
#include "../lib/list.h"
#include "../lib/map-reduce.h"
#include "task.h"

#define TODO()\
do{\
    extern int printf(char *, ...);\
    printf("Add your code here: file %s, line %d\n", __FILE__, __LINE__);\
}while(0)





extern list_t (*mr_map)(char *);
extern char *(*mr_reduce)(char *, list_t);

const char *TMP_DIR = "/tmp/mth_data";
const char *OUTPUT_DIR = "./output";

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

void map_phase(task_t *task, char *map_name)
{
    // map phase
    // Exercise:
    // Add your code here:
    TODO();

}


void reduce_phase(task_t *task, char *reduce_name)
{
    // reduce phase
    // Exercise:
    // Add your code here:
    TODO();

}