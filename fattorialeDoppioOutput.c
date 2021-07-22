/**
Scrivere un programma in C e Posix che:
1) crea un thread A ed esegue una procedura che calcola il fattoriale di n(fornito dall'utente) e restituisce un codice di completamenteto 0<=RET<=2,
generato casualmente. 
Se:
	-RET = 0, il programma termina.
	-RET <> 0, crea due thread, B e C, che generano 1000 numeri interi che inseriscono in modo concorrente in un array di 1000 elementi e terminano l'esecuzione. 
    Successivamente, un thread D, che era in attesa che B e C terminassero, stampa il thread, B o C, che ha inserito piÃ¹ interi nell'array e termina l'esecuzione. 
**/


#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <time.h>
#include <fcntl.h>

int numeroInput = 0;
void *returnValue;
int insertA=0;
int insertB=0;
int indice=0;
int buffer[1000];

sem_t entro;
sem_t completato;

pthread_t A;
pthread_t B;
pthread_t C;
pthread_t D;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *execFattoriale(void *arg);
void *generateNumber(void *arg);
void *finito(void *arg);
int fattoriale(int numeroInput);

int main(int argc, char **argv){
    srand(time(NULL));

    numeroInput = atoi(argv[1]);

    sem_init(&completato, 0, 0);
    sem_init(&entro, 0, 1);
    
    pthread_create(&A, NULL, execFattoriale, NULL);
    pthread_join(A, &returnValue);
    printf("%ld\n", (long)returnValue);
    if((long)returnValue==0){
        return 0;
    }
    else{
        pthread_create(&D, NULL, finito, NULL);
        pthread_create(&B, NULL, generateNumber, NULL);
        pthread_create(&C, NULL, generateNumber, NULL);
        pthread_join(B, NULL);
        pthread_join(C, NULL);
        pthread_join(D, NULL);
    }

    return 0;

}

void *generateNumber(void *arg){
    while (1)
    {
        
        
        if(indice < 1000){
            sem_wait(&entro);
            pthread_mutex_lock(&mutex);
            buffer[indice]=rand()%100;
            printf("Indice: %d N: %d\n", indice, buffer[indice]);
            indice++;
            printf("%d\n", indice);
            if(pthread_self()==B){
                insertB++;
            }else{
                insertA++;
            }
            pthread_mutex_unlock(&mutex);
            sem_post(&entro);
        }
        else{
            sem_post(&completato);
            pthread_exit(NULL); 
        }

    }
       
}

void *finito(void *arg){
    while(1){
        sem_wait(&completato);
        if(insertA > insertB){
            printf("il thread A ha inserito piu numeri. N_A: %d, N_B: %d\n", insertA, insertB);
            pthread_exit(NULL);
        }
        else{
            printf(" il thread B ha inserito piu numeri.  N_A: %d, N_B: %d\n", insertA, insertB);
            pthread_exit(NULL);
        }
    }
}

void *execFattoriale(void *arg){
    fattoriale(numeroInput);
    int returnValue;
    
    returnValue = rand()%3;
    pthread_exit((void *)(long)returnValue);
}

int fattoriale(int numeroInput)
{
  if (numeroInput < 0) return -1; /* Fattoriale non e' definito per interi negativi! */

  if (numeroInput == 0) return 1;
  else return numeroInput*fattoriale(numeroInput-1);
}