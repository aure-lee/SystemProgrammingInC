#include "mini-lib.h"

// see:
//https://blog.rchapman.org/posts/Linux_System_Call_Table_for_x86_64/

// syscall number for x86-64, in rax
#define SYS_WRITE 1
#define SYS_SLEEP 35
#define SYS_ALARM 37
#define SYS_EXIT 60

// arguments: rdi, rsi, rdx, r10, r8, r9


#define CALL2(n) "movq $"#n", %%rax\n"
#define CALL(n) CALL2(n)

int strlen(char *s){
    int n = 0;
    while(*s++)
        n++;
    return n;
}

int printf(const char *s, ...){
    // Your code here:
    // va_list args; // 申明一个va_list类型对象，用于参数遍历
    // va_start(args, s);

    // while (*s) {
    //     if (*s == '%') {
    //         ++s;
    //         switch (*s)
    //         {
    //         case 's':
    //             puts(va_arg(args, char *));
    //             break;
            
    //         default:
    //             putchar(*s);
    //             break;
    //         }
    //     }
    //     else 
    //         putchar(*s);
    //     ++s;
    // }

    // va_end(args);
    // TODO();
    return 0;
}

int puts(char *s) {
    long n = strlen(s);
    long r;
    asm(CALL(SYS_WRITE)
       "movq $1, %%rdi\n"
       "movq %1, %%rsi\n"
       "movq %2, %%rdx\n"
       "syscall\n"
       "movq %%rax, %0\n"
       : "=r"(r)
       : "r"(s), "r"(n)
       : "%rax", "%rdi", "%rsi", "%rdx");
    return (int)r;
}


void exit(int status){
    // Your code here:
    long r;
    asm(CALL(SYS_EXIT)
        "movq %0, %%rdi\n"
        "syscall\n"
        "movq %%rax, %0\n"
        : "=r"(r)
        : "r"(status)
        : "%rax", "%rdi");
    // TODO();
}

int alarm(unsigned int seconds){
    // Your code here:
    long r;
    asm(CALL(SYS_ALARM)
        "movq %0, %%rdi\n"
        "syscall\n"
        "movq %%rax, %0\n"
        : "=r"(r)
        : "r"(seconds)
        : "%rax", "%rdi");
    return 0;
    // TODO();
}


int sleep(struct timespec *ts, struct timespec *rem){
    // Your code here:
    long r;
    asm(CALL(SYS_SLEEP)
        "movq %0, %%rdi\n"
        "movq %1, %%rsi\n"
        "syscall\n"
        "movq %%rax, %0\n"
        : "=r"(r)
        : "r"(ts), "r"(rem)
        : "%rax", "%rdi", "%rsi");
    return 0;
    // TODO();
}

int putchar(char c)
{
    long r;
    asm(CALL(SYS_WRITE)
       "movq $1, %%rdi\n"
       "movq %1, %%rsi\n"
       "movq $1, %%rdx\n"
       "syscall\n"
       "movq %%rax, %0\n"
       : "=r"(r)
       : "r"(&c)
       : "%rax", "%rdi", "%rsi", "%rdx");
    return 0;
}

