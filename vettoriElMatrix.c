/* Si realizzi un programma C e Posix in ambiente Linux che, impiegando
la libreria Pthread, crei una matrice di interi generati casualmente
di dimensioni nxn (con n numero pari fornito da riga di comando), e
provveda poi a lanciare n thread (uno per riga). Ciascun thread legge
un valore casuale sulla propria riga e lo inserisce, a turno (un
inserimento al secondo), in un vettore di n elementi. 
Un ulteriore thread attende il riempimento del vettore, lo stampa, e termina.
Si usino semafori Posix per memoria e variabili di condizione. */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>


int **matrix;
pthread_t *lettori;
pthread_t master;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condition = PTHREAD_COND_INITIALIZER;
sem_t contatore;
int *array;
int n;
int indice = 0;

void riempiMatrici(int **matrix);
void *leggiScrivi(void *arg);
void *stmpResult(void *arg);

int main(int argc, char **argv){
    srand(time(NULL));

    if(argc > 1){
        n = atoi(argv[1]);
        if(n%2!=0){
            fprintf(stderr, "Inserisci un numero pari!");
            exit(-1);
        }
    }else{
        fprintf(stderr, "Inserisci un numero!");
        exit(-2);
    }
    
    int count[n];

    sem_init(&contatore, 0, n);

    lettori = (pthread_t *)malloc(n * sizeof(pthread_t));
    matrix = (int **)malloc(n * sizeof(int *));
    array = (int *)calloc(n, sizeof(int));

    for(int k = 0; k < n; k++){
        matrix[k]= (int *)malloc(n * sizeof(int));
    }

    riempiMatrici(matrix);

    for(int i = 0; i < n; i++){
        count[i]=i;
        pthread_create(&lettori[i], NULL, leggiScrivi, &count[i]);
    }

    pthread_create(&master, NULL, stmpResult, NULL);

    for(int j = 0; j < n; j++){
        pthread_join(lettori[j], NULL);
    }

    pthread_join(master, NULL);

    sem_destroy(&contatore);
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&condition);

    return 0;    
}

void *leggiScrivi(void *arg){
    int i = (*(int *)arg);
    int col = rand()%n;
    sem_wait(&contatore);
    pthread_mutex_lock(&mutex);
        fprintf(stdout,"indice %d, possiede elemento -> %d\n", indice, matrix[i][col]);
        array[indice]=matrix[i][col];
        indice++;
        printf("Indice ->%d\n", indice);
        sleep(1);
        if(indice == n){
            pthread_cond_signal(&condition); //Se il numero dell'indice è uguale ad N allora invio il segnale al thread Master
        }
    pthread_mutex_unlock(&mutex);
    
    pthread_exit(NULL);
}

void *stmpResult(void *arg){
    pthread_mutex_lock(&mutex);
        while(indice < n)
            pthread_cond_wait(&condition, &mutex); //Aspetto fintanto che la condizione non si è realizzata
        for(int i = 0; i < n; i++){
            sem_post(&contatore);
            printf("Elemento %d del vettore = %d\n",i, array[i]);
        }
    pthread_mutex_unlock(&mutex);
}

void riempiMatrici(int **matrix){
    for(int i = 0; i < n; i++){
        for(int j = 0; j < n; j++){
            matrix[i][j]=rand()%20;
        }
    }
}