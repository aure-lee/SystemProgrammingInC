#include "ast.h"
#include "alloc.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

struct node *make_node(char *data, struct node *next){
    struct node *t = malloc(sizeof(*t));
    t->data = data;
    t->next = next;
    return t;
}

// constructor of each AST node
Cmd_t Cmd_Seq_new(Cmd_t left, Cmd_t right){
    Cmd_Seq cmd;
    NEW(cmd);
    cmd->type = CMD_SEQ;
    cmd->left = left;
    cmd->right = right;

    return (Cmd_t)cmd;
}

Cmd_t Cmd_Back_new(Cmd_t back){
    // TODO
}

Cmd_t Cmd_Pipe_new(Cmd_t left, Cmd_t right){
    // TODO
}

Cmd_t Cmd_Redir_new(Cmd_t left, Cmd_t right, int fd){
    // TODO
}

Cmd_t Cmd_Atom_new(struct node *node){
    Cmd_Atom cmd;
    NEW(cmd);
    cmd->type = CMD_ATOM;
    cmd->node = node;

    return (Cmd_t)cmd;
}

// print AST
void Cmd_print(struct Cmd_t *cmd){
    switch(cmd->type){
        case CMD_ATOM: {
            struct Cmd_Atom  *t = (struct Cmd_Atom *) cmd;
            struct node *node = t->node;
            while (node) {
                printf("%s ", node->data);
                node = node->next;
            }
            break;
        }

        case CMD_SEQ: {
            printf("enter\n");
            struct Cmd_Seq *t = (struct Cmd_Seq *) cmd;
            printf("enter\n");
            Cmd_t left = t->left;
            Cmd_t right = t->right;

            Cmd_print(left);
            printf("; ");
            Cmd_print(right);
            break;
        }

        case CMD_BACK: {
            // TODO
        }

        case CMD_PIPE: {
            // TODO
        }

        case CMD_REDIR: {
            // TODO
        }
        default:
            break;
    }
}

// run cmd
void Cmd_run(struct Cmd_t *cmd){
        switch(cmd->type){
            case CMD_ATOM: {
                char *arg[10] = {0};
                int i = 0;
                struct Cmd_Atom  *t = (struct Cmd_Atom *) cmd;
                struct node *node = t->node;
                while(node){
                    arg[i] = node->data;
                    node = node->next;
                    i++;
                }
                char *root = "/bin/";
                char binPath[50];
                strcpy(binPath, root);
                strcat(binPath, arg[0]);
                if(execv(binPath, arg) == -1){
                    char *path= "/usr/bin/";
                    char usrBinPath[50];
                    strcpy(usrBinPath, path);
                    strcat(usrBinPath, arg[0]);
                    if(execv(usrBinPath, arg) == -1){
                        fprintf(stderr,"cannot run command, check your input.\n");
                    }
                }
                break;
            }

            case CMD_SEQ: {
                struct Cmd_Seq *t = (struct Cmd_Seq *) cmd;
                Cmd_t left = t->left;
                Cmd_t right = t->right;

                if(fork() == 0){
                    Cmd_run(left);
                }
                wait(0);
                Cmd_run(right);
                break;
            }

            case CMD_BACK: {
                // TODO
            }

            case CMD_PIPE: {
                // TODO
            }

            case CMD_REDIR: {
                // TODO
            }

            default:
                break;
        }

    exit(0);
}
