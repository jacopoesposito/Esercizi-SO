/* Esercizio: scrivere un programma che esegua il comando di
shell “ls| sort| grep <pat>” con tre processi distinti */


#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define PIPE_READ 0
#define PIPE_WRITE 1

int p1, p2, p3;
int pfd1[2], pfd2[2];

int main(int argc, char **argv){

    pipe(pfd1); //Apro la pipe PFD1
    pipe(pfd2); //Apro la pipe PFD2
    
    if((p1 = fork())==0){
        close(pfd1[PIPE_WRITE]); 
        close(pfd1[PIPE_READ]);
        close(pfd2[PIPE_WRITE]);
        dup2(pfd2[PIPE_READ], STDIN_FILENO);
        close(pfd2[PIPE_READ]);
        execlp("grep", "grep", argv[1], NULL);
        perror("Grep error");

        exit(-1);
    }
    else if((p2 = fork())==0){
        close(pfd1[PIPE_WRITE]);
        close(pfd2[PIPE_READ]);
        dup2(pfd1[PIPE_READ], STDIN_FILENO);
        close(pfd1[PIPE_READ]);
        dup2(pfd2[PIPE_WRITE], STDOUT_FILENO);
        close(pfd2[PIPE_WRITE]);
        execlp("sort", "sort", NULL);
        perror("Sort Error");

        exit(-2);
    }
    else if((p3 = fork())==0){
        close(pfd1[PIPE_READ]);
        close(pfd2[PIPE_WRITE]);
        close(pfd2[PIPE_READ]);
        dup2(pfd1[PIPE_WRITE], STDOUT_FILENO);
        close(pfd1[PIPE_WRITE]);
        execlp("ls", "ls", NULL);

        perror("LS error");

        exit(-3);
    }

    close(pfd1[PIPE_READ]);
    close(pfd1[PIPE_WRITE]);
    close(pfd2[PIPE_READ]);
    close(pfd2[PIPE_WRITE]);
    
    
    
    waitpid(p1, NULL, 0);
    waitpid(p2, NULL, 0);
    waitpid(p3, NULL, 0);

    exit(0);
}