/* Esercizio: scrivere un programma che esegua il comando di
shell “ls- R | grep <pat>” dove <pat> è un pattern inserito
dall’utente */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define PIPE_READ 0
#define PIPE_WRITE 1

int main(int argc, char **argv){
    int pfd[2], pid;

    if(pipe(pfd) == -1){
        perror("pipe() failed");
        exit(-1);
    }
    if((pid=fork()) < 0){
        perror("Fork fallita jamm");
        exit(-2);
    }
    if(pid == 0){ //Tu hai ucciso mio padre
        close(pfd[PIPE_WRITE]); //Chiudo la PIPE in scrittura per il figlio
        dup2(pfd[PIPE_READ], STDIN_FILENO); 
        close(pfd[PIPE_READ]); //Chiudo la PIPE in lettura
        execlp("grep", "grep", *( argv + 1), NULL); //Il figlio esegue il comando GREP con il pattern indicato da riga di comando 
        fprintf(stderr, "ERRORE : exec().\n");
        exit(-3);
    }
    else{ //No, io sono tuo padre!
        close(pfd[PIPE_READ]); //Chiudo la PIPE in lettura per il padre
        dup2(pfd[PIPE_WRITE], STDOUT_FILENO);
        close(pfd[PIPE_WRITE]); //Chiudo la pipe in scrittura per il padre
        execlp("ls", "ls", "-R", NULL); //Il padre esegue il comando LS ricorsivo
        fprintf(stderr, "ERRORE : exec().\n");
        exit(-4);
    }

    exit(0);
}