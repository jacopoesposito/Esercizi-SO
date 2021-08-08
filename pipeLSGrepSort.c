/* Scrivere un programma C e Posix sotto Linux che implementi la seguente pipeline acquisita in input da
riga di comando: ls | grep <stringa> | sort -r.  */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define PIPE_READ 0
#define PIPE_WRITE 1

int main(int argc, char **argv){

    int pfd1[2], pfd2[2];
    int pid1, pid2;

    pipe(pfd1);
    pipe(pfd2);

    if((pid1=fork())==0){
        close(pfd2[PIPE_WRITE]);
        close(pfd1[PIPE_WRITE]);
        dup2(pfd2[PIPE_READ], STDIN_FILENO);
        close(pfd1[PIPE_READ]);
        execlp("sort", "sort", "-r", NULL);
        perror("sort error");
        exit(-1);
    }
    else if((pid2=fork())==0){
        close(pfd2[PIPE_READ]);
        dup2(pfd2[PIPE_WRITE], STDOUT_FILENO);
        close(pfd1[PIPE_WRITE]);
        dup2(pfd1[PIPE_READ], STDIN_FILENO);
        close(pfd1[PIPE_READ]);
        execlp("grep", "grep", argv[1], NULL);
        perror("grep error");
        exit(-2);
    }else if(pid1 != 0 && pid2 != 0){
        close(pfd2[PIPE_WRITE]);
        close(pfd2[PIPE_READ]);
        close(pfd1[PIPE_READ]);
        dup2(pfd1[PIPE_WRITE], STDOUT_FILENO);
        close(pfd1[PIPE_WRITE]);
        execlp("ls", "ls", NULL);
        perror("ls error");
        exit(-3);
    }

    close(pfd1[PIPE_READ]);
    close(pfd1[PIPE_WRITE]);
    close(pfd2[PIPE_READ]);
    close(pfd2[PIPE_WRITE]);
    
    
    
    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);

    return 0;
}