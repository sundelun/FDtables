#include "header.h"

void output_txt(Node *node){
    FILE *file;
    file = fopen("composite.txt","w");
    if (!file){
        perror("Error open file");
        return;
    }
    int i = 0;
    Node *ite = node;
    while (ite != NULL){
        fprintf(file, "%d\t%d\t%d\t%s\t%lu\n", i, ite->pid, ite->fd, ite->name, (unsigned long)ite->inode);
        i++;
        ite = ite->next; 
    }
    fclose(file);
}

void output_bin(Node *node){
    FILE *file;
    file = fopen("composite.bin","wb");
    if (!file){
        perror("Error open file");
        return;
    }
    int i = 0;
    Node *ite = node;
    while (ite != NULL){
        fwrite(" ", sizeof(char), 1, file); 
        fwrite(&i, sizeof(int), 1, file);
        fwrite("\t", sizeof(char), 1, file); 
        fwrite(&(ite->pid), sizeof(int), 1, file); 
        fwrite("\t", sizeof(char), 1, file);
        fwrite(&(ite->fd), sizeof(int), 1, file); 
        fwrite("\t", sizeof(char), 1, file);
        fwrite(ite->name, sizeof(char), strlen(ite->name), file); 
        fwrite("\t", sizeof(char), 1, file);
        fwrite(&(ite->inode), sizeof(ino_t), 1, file); 
        fwrite("\n", sizeof(char), 1, file);
        ite = ite->next;
    }
    fclose(file);
}