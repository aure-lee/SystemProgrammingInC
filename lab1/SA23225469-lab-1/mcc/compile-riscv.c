#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include "compile.h"
#include "set.h"


// prototypes
static void compile_exp(Exp_t exp);
static void compile_stm(Stm_t stm);
static int Deep_stm(Stm_t stm);
static int Deep_exp(Exp_t exp);
static void PUSH(void);
static void POP(void);

static int stackoffset = 0;


static char instr[1024];
static int local_fd = 1;


// emit instructions
static void emit(char *s){
    write(local_fd, s, strlen(s));
}

static void emit_vars() {
    char *var = 0;
    emit("\n\t.section\t.sdata\n");
    while ((var=set_next())){
        sprintf(instr, "\t.global\t%s\n", var);
        emit(instr);
        emit("\t.p2align\t2\n");
        sprintf(instr,"\t.type\t%s, @object\n", var);
        emit(instr);
        sprintf(instr,"%s:\n", var);
        emit(instr);
        emit("\t.word\t0\n");
        sprintf(instr,"\t.size\t%s, 4\n", var);
        emit(instr);
    }
}

static void PUSH(){
    sprintf(instr,"\tsw\ta1, %d(sp)\n", 4*stackoffset);
    emit(instr);
    stackoffset++;
}

static void POP(){
    // TODO
}

static void compile_exp(Exp_t exp){
    switch (exp->type) {
        case EXP_NUM:{
            Exp_Num e = (Exp_Num)exp;
            sprintf(instr, "\tli\ta1, %d\n", e->num);
            emit(instr);
            break;
        }
        case EXP_VAR: {
            // TODO
            break;
        }
        case EXP_ADD: {
            Exp_Add e = (Exp_Add) exp;
            compile_exp(e->left);
            PUSH();
            compile_exp(e->right);
            POP();
            emit("\taddw\ta1, a1, a2\n");
            break;
        }
        case EXP_SUB: {
            // TODO
            break;
        }
        case EXP_TIMES: {
            // TODO
            break;
        }
        case EXP_DIV: {
            Exp_Div e = (Exp_Div) exp;
            compile_exp(e->left);
            PUSH();
            compile_exp(e->right);
            POP();
            emit("\tdivw\ta1, a2, a1\n");
            break;
        }
        default:{
            emit("\t# Unknown expression type\n");
            break;
        }
    }
}

static void compile_stm(Stm_t stm){
    switch (stm->type) {
        case STM_ASSIGN: {
            Stm_Assign s = (Stm_Assign) stm;
            set_add(s->x);
            compile_exp(s->exp);
            sprintf(instr, "\tlui\ta3, %%hi(%s)\n", s->x);
            emit(instr);
            sprintf(instr, "\tsw\ta1, %%lo(%s)(a3)\n", s->x);
            emit(instr);
            break;
        }
        case STM_PRINT: {
            // TODO
            break;
        }
        case CMD_SEQ: {
            // TODO
            break;
        }
        default: {
            emit("# Unknown statement type\n");
            break;
        }
    }
}

// compile a whole program
void riscv_compile(Stm_t prog, int fd){


    local_fd = fd;
    // prolog
    emit("# Auto-generated by mcc. Do not modify.\n");
    emit("\t.file \"input.txt\"\n");
    emit("\t.text\n");
    emit("mcc_format:\n");
    emit("\t.asciz	\"%d\\n\"\n");
    emit("\t.globl main\n");
    emit("\t.type main, @function\n");
    emit("main:\n");

    int stacksize = 20 + 4 * (Deep_stm(prog));

    sprintf(instr, "\taddi\tsp, sp, -%d\n", stacksize);
    emit(instr);
    sprintf(instr, "\tsd\tra, %d(sp)\n", stacksize-8);
    emit(instr);
    sprintf(instr, "\tsd\ts0, %d(sp)\n", stacksize-16);
    emit(instr);
    sprintf(instr, "\taddi\ts0, sp, %d\n", stacksize);
    emit(instr);

    compile_stm(prog);

    sprintf(instr, "\tld\tra, %d(sp)\n", stacksize-8);
    emit(instr);
    sprintf(instr, "\tld\ts0, %d(sp)\n", stacksize-16);
    emit(instr);
    sprintf(instr, "\taddi\tsp, sp, %d\n", stacksize);
    emit(instr);

    emit("\tret\n");

    emit_vars();

    // epilog
    emit("# End and bye.\n\n");

}

int max(int x, int y){
    return x>y ? x : y;
}

int Deep_stm(Stm_t stm){
    switch (stm->type){
    case STM_PRINT :{
        Stm_Print s = (Stm_Print) stm;
        return Deep_exp(s->exp);
    }
    case STM_ASSIGN :{
        Stm_Assign s = (Stm_Assign) stm;
        return Deep_exp(s->exp);
    }
    case CMD_SEQ :{
        Stm_Seq s = (Stm_Seq) stm;
        return max(Deep_stm(s->left), Deep_stm(s->right));
    }
    default: {
        emit("\t// Unknown statement type\n");
        return 0;
    }
    }
}

int Deep_exp(Exp_t exp){
    switch (exp->type)
    {
    case EXP_NUM :
    case EXP_VAR :
        return 0;
    case EXP_ADD :
    case EXP_SUB :
    case EXP_TIMES :
    case EXP_DIV :{
        Exp_Div e = (Exp_Div)exp;
        return max(Deep_exp(e->left), Deep_exp(e->right)) + 1;
    }
    default:
        emit("\t// Unknown expression type\n");
        return 0;
    }
}
