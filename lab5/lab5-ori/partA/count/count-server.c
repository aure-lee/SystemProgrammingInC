
#define TODO()\
do{\
    extern int printf(char *, ...);\
    printf("Add your code here: file %s, line %d\n", __FILE__, __LINE__);\
}while(0)



int count = 0;

int increment() {
    count++;
    return count;
}

int decrement() {
    count --;
    return count;
}

int get_count() {
    return count;
}