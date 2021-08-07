/* Si realizzi un programma C e Posix in ambiente Linux che, impiegando la libreria Pthread, generi tre
    thread. I primi due thread sommano 1000 numeri generati casualmente ed ogni volta incrementano un
    contatore. Il terzo thread attende che il contatore incrementato dai due thread raggiunga un valore limite
    fornito da riga di comando, notifica l’avvenuta condizione e termina. Utilizzare le variabili condizione. 
*/


#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>


int indice;
int n;
int somma = 0;
pthread_t *produttori;
pthread_t master;
pthread_mutex_t mutex;
pthread_cond_t condition = PTHREAD_COND_INITIALIZER;

void *checking(void *arg);
void *produci(void *arg);

int main(int argc, char **argv){
    srand(time(NULL));

    if(argc < 2){
        fprintf(stderr, "Errore Input");
    }

    n = atoi(argv[1]);

    produttori = (pthread_t *)malloc(n * sizeof(pthread_t));

    pthread_create(&master, NULL, checking, NULL);

    for(int i = 0; i < 2; i++){
        pthread_create(&produttori[i], NULL, produci, NULL);
    }

    for(int j = 0; j < 2; j++){
        pthread_join(produttori[j], NULL);
    }

    pthread_join(master, NULL);
    pthread_cond_destroy(&condition);

    return 0;
}

void *produci(void *arg){
    for(int i = 0; i < 999; i++){
    pthread_mutex_lock(&mutex);
        if(indice==n){
            pthread_cond_signal(&condition);
            pthread_mutex_unlock(&mutex);
            pthread_exit(NULL);
        }
        somma+=rand()%20;
        indice++;
    pthread_mutex_unlock(&mutex);
    }
}

void *checking(void *arg){
    pthread_mutex_lock(&mutex);
        while(indice < n){
            pthread_cond_wait(&condition, &mutex);
        }
        printf("Finito (っ˘ω˘ς ) somma->%d\n", somma);
    pthread_mutex_unlock(&mutex);
}