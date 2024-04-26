#include "header.h"

void composite_print(Node *node){
    Node *ite = node;
    int i = 0;
    printf("\t PID\tFD\tFilename\tInode\n");
    printf("\t=============================================\n");
    while (ite != NULL){
        printf(" %d\t%d\t%d\t%s\t%lu\n", i, ite->pid, ite->fd, ite->name, (unsigned long)ite->inode);
        i++;
        ite = ite->next; 
    }
    printf("\t=============================================\n\n");
}

void per_process_print(Node *node){
    Node *ite = node;
    printf("\t PID\tFD\n");
    printf("\t ============\n");
    while (ite != NULL){
        printf("\t %-7d%-7d\n", ite->pid, ite->fd);
        ite = ite->next;
    }
    printf("\t============\n\n");
}

void system_wide_print(Node *node){
    Node *ite = node;
    printf("\t PID\tFD\tFilename\n");
    printf("\t====================================\n");
    while (ite != NULL){
        printf("\t %d\t%d\t%s\n", ite->pid, ite->fd, ite->name);
        ite = ite->next;
    }
    printf("\t====================================\n\n");
}

void vnodes_print(Node *node){
    Node *ite = node;
    printf("\t FD\tInode\n");
    printf("\t====================================\n");
    while (ite != NULL){
        printf("\t %d\t%lu\n",ite->fd, (unsigned long)ite->inode);
        ite = ite->next; 
    }
    printf("\t====================================\n");
}

void threshold_print(Node *node){
    Node *ite = NULL;
    ite = node;
    printf("## Offending processes:\n");
    while (ite != NULL){
        printf("%d (%d)", ite->pid, ite->fd_cnt);
        if (ite->next !=NULL) printf(", ");
        ite = ite->next;
    }
    printf("\n");
}