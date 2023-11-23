#ifndef LIST_H
#define LIST_H

#define TODO()\
do{\
    extern int printf(char *, ...);\
    printf("Add your code here: file %s, line %d\n", __FILE__, __LINE__);\
}while(0)




typedef struct List_t *list_t;
struct List_t{
    void **containter;
    int size; // maximum size
    int last; // always point to position to hold the next item
};

list_t list_new();
void list_append(list_t list, void *data);
void *list_nth(list_t list, int n);
void list_print(list_t, void (*f)(void *));

#endif
