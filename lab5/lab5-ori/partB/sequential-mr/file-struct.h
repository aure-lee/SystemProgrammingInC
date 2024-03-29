
#define TODO()\
do{\
    extern int printf(char *, ...);\
    printf("Add your code here: file %s, line %d\n", __FILE__, __LINE__);\
}while(0)



// file-struct.h
#ifndef FILE_STRUCT_H
#define FILE_STRUCT_H
#include <stdio.h>
#include "../lib/list.h"

typedef struct {
    char **file_paths;  
    int num_fps;
} inter_res;

typedef struct {
    char *fname;
    inter_res *inter;
} file_list;

typedef struct {
    file_list *files;
    int num_files;
    const char *tmp_dir;
    const char *output_dir;
} fnode;

fnode *init_fnode(int nInput, const char *argv[], int nFiles, const char *tmp_dir, const char *output_dir);
void free_fnode(fnode *node, int numFiles);

#endif
