/* Si realizzi un programma C sotto linux che preso un intero N come parametro da riga 
di comando (compreso tra 1 e 100) generi due figli f1 e f2 e proceda come segue.
Se il parametro di N è pari, il programma invia il segnale SIGUSR1 al figlio f1 che calcola la somma tra il
valore del suo PID e quello del processo padre. Il processo f2 stamperà solo il suo PID.
Se il parametro N è dispari, il programma invia il segnale SIGUSR2 al figlio f2 che calcola
la differenza tra il valore del PID del processo padre e il suo PID. Il processo f1 stamperà solo il
suo PID. Alla fine, il processo principale calcolerà e stamperà il prodototo dei PID dei due processi 
figli f1 e f2 e terminerò stampando un messaggio di saluto. */


#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>

int N;
int pid = 0;
pid_t f1;
pid_t f2;
pid_t summedPid;
pid_t fatherPid;

struct sigaction new;

static void sumPid(int signo);

int main(int argc, char **argv){
    
    char *a = argv[1];
    N = atoi(a);
    
    sigset_t block_mask;

    sigemptyset (&block_mask);
    /* Block other terminal-generated signals while handler runs. */
    sigaddset (&block_mask, SIGINT);
    sigaddset (&block_mask, SIGQUIT);
    new.sa_handler = sumPid;
    new.sa_mask = block_mask;
    new.sa_flags = 0;
    
    sigaction(SIGUSR1, &new, NULL);
    sigaction(SIGUSR2, &new, NULL);
    fatherPid = getpid();

    if(N > 1 && N < 100){
        f1 = fork();
        if(f1 != 0){
            f2 = fork();
        }
        if(N%2==0){
            if(f1 != 0 && f2 != 0){
                //printf("%d\n", f1);
                kill(f1, SIGUSR1);
            }
            if(f2 == 0 && f1 != 0){
                printf("Pid di f2: %d\n", getpid());
            }
        }
        else{
            if(f1 != 0 && f2 != 0){
                kill(f2, SIGUSR2);
            }
            if(f1 == 0){
                printf("Pid di f1: %d\n", getpid());
            }
        }
    }
    else{
        printf("Errore input!");
    }

    wait(NULL);
    wait(NULL);

    return 0;
}

static void sumPid(int signo){
    if(signo == SIGUSR1){ 
        summedPid = getpid() + fatherPid;
        printf("PID %d ha calcolato la somma %d\n", getpid(), summedPid);
    }
    if(signo == SIGUSR2) {
        summedPid = getpid() + fatherPid;
        printf("PID %d ha calcolato la somma %d\n", getpid(), summedPid);
    }
}