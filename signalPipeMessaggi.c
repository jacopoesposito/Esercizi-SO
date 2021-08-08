/* Realizzare un programma in C e Posix che realizza il seguente comportamento: un processo P padre crea
un processo P figlio il quale inizialmente attende un segnale SIGUSR1 oppure SIGUSR2 dal processo P padre.
Il processo Ppadre , dopo aver atteso un intervallo di durata casuale, invia a Pfiglio un segnale scelto
casualmente tra SIGUSR1 e SIGUSR2 per poi inviare, attraverso una pipe, il numero (1 o 2 ) del segnale
inviato; Il processo Pfiglio, ricevuto il segnale e letto il messaggio dalla pipe, visualizza il segnale e il
messaggio ricevuti. */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>

int pfd[2];

void invioPipe(int number);
static void signalHandler(int signo);

int main(int argc, char **argv){

    pid_t pidFiglio;
    pipe(pfd);

    srand(time(NULL));
    signal(SIGUSR1, signalHandler);
    signal(SIGUSR2, signalHandler);

    if((pidFiglio = fork())==0){
        pause();
    }else{
        int random = 1+rand()%2;
        sleep(random);
        printf("Numero scelto %d, PID %d\n", random, getpid());
        if(random == 1){
            invioPipe(random);
            kill(pidFiglio, SIGUSR1); //Invio il segnale SIGUSR1
            
        }else{
            invioPipe(random);
            kill(pidFiglio, SIGUSR2);  //Invio il segnale SIGUSR2
        }
        
        wait(NULL);
        return 0;
    }
}

void invioPipe(int number){
    close(pfd[0]);//Chiudo la pipe in lettura  
    write(pfd[1], &number, sizeof(number));//Scrivo il contenuto di number nella pipe
    close(pfd[1]);//Chiudo la pipe in scrittura
}

static void signalHandler(int signo){
    if(signo == SIGUSR1 || signo == SIGUSR2){
        int value;
        close(pfd[1]);//Chiudo la pipe in scrittura 
        read(pfd[0], &value, sizeof(value));//Leggo il contenuto della pipe e lo inserisco nella variabile value
        close(pfd[0]);//Chiudo la pipe in lettura
        printf("PID->%ld\nMessaggio pipe -> %d\nSegnale ricevuto -> %ld\n", getpid(), value, signo);
        exit(0);
    }    
}