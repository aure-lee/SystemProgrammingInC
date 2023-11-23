
#define TODO()\
do{\
    extern int printf(char *, ...);\
    printf("Add your code here: file %s, line %d\n", __FILE__, __LINE__);\
}while(0)



void put(char *key, char *value);
char *get(char *key);
void delete(char *key);
