#include "task.h"

#define TODO()\
do{\
    extern int printf(char *, ...);\
    printf("Add your code here: file %s, line %d\n", __FILE__, __LINE__);\
}while(0)




void map_phase(task_t *task, char *map_name);
void reduce_phase(task_t *task, char *reduce_name);