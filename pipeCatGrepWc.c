/* Scrivere un programma in C e Posix che implementi la seguente pipeline di comandi come verrebbero gestiti da
una shell Linux: $ cat /etc/passwd | grep /bin/bash | wc -c */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

#define PIPE_READ 0
#define PIPE_WRITE 1

int main(int argc, char **argv){

    int pfd1[2], pfd2[2];
    int pid1, pid2;

    pipe(pfd1);
    pipe(pfd2);

    if((pid1 = fork())==0){
        close(pfd2[PIPE_WRITE]);
        close(pfd1[PIPE_READ]);
        close(pfd1[PIPE_WRITE]);
        dup2(pfd2[PIPE_READ], STDIN_FILENO);
        close(pfd2[PIPE_READ]);
        execlp("wc", "wc", "-c", NULL);
        perror("wc error");
        exit(-1);
    }else if((pid2 = fork())==0){
        close(pfd2[PIPE_READ]);
        dup2(pfd2[PIPE_WRITE], STDOUT_FILENO);
        close(pfd1[PIPE_WRITE]);
        dup2(pfd1[PIPE_READ], STDIN_FILENO);
        close(pfd1[PIPE_READ]);
        execlp("grep", "grep", "/bin/bash", NULL);
        perror("grep error");
        exit(-2);
    }else if(pid1 != 0 && pid2 != 0){
        close(pfd2[PIPE_READ]);
        close(pfd2[PIPE_WRITE]);
        close(pfd1[PIPE_READ]);
        dup2(pfd1[PIPE_WRITE], STDOUT_FILENO);
        close(pfd1[PIPE_WRITE]);
        execlp("cat", "cat", "/etc/passwd", NULL);
        perror("grep error");
        exit(-2);
    }

    close(pfd1[PIPE_READ]);
    close(pfd1[PIPE_WRITE]);
    close(pfd2[PIPE_READ]);
    close(pfd2[PIPE_WRITE]);
    
    
    
    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);

    return 0;

}
