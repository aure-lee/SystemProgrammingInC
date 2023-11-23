
#define TODO()\
do{\
    extern int printf(char *, ...);\
    printf("Add your code here: file %s, line %d\n", __FILE__, __LINE__);\
}while(0)




char **map_phase(char *map_name, int reduce_size);
void reduce_phase(char *reduce_name, char **input_files, int file_size);