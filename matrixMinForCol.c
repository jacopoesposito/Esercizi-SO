/*
    Si realizzi un programma in C e Posix in ambiente Linux che, impiegando la libreria Pthread, 
    creai una matrice di interi di dimensione nxn con n passato come argomento da riga di comando, 
    e provveda a sommare concorrentemente ciascuna colonne della matrice e a determinarne il minimo 
    il cui valore dev'essere assegnato ad una variabile globale minimo e poi al termine stampato da un altro thread.
*/


#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h> 
#include <ctype.h>
#include <time.h>
#include <limits.h>

int n;
int **matrix;
int valueMin = INT_MAX;
int indice = 0;
pthread_t *produttore;
pthread_t master;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condition = PTHREAD_COND_INITIALIZER;


void riempiMatrice(int **matrix);
void stmpMatrice(int **matrix);
void *produci(void *arg);
void *stampa(void *arg);

int main(int argc, char **argv){
    
    srand(time(NULL));

    if(argc < 2){
        fprintf(stderr, "INPUT MANCANTE!");
        exit(-1);
    }

    n = atoi(argv[1]);

    int count[n];

    matrix = (int **)malloc(n * sizeof(int *));
    produttore = (pthread_t *)malloc(n * sizeof(pthread_t));

    for(int i = 0; i < n; i++){
        matrix[i]=(int *)malloc(n * sizeof(int));
    }

    riempiMatrice(matrix);
    stmpMatrice(matrix);
    printf("\n");

    for(int j = 0; j < n; j++){
        count[j]=j;
        pthread_create(&produttore[j], NULL, produci, &count[j]);
    }

    pthread_create(&master, NULL, stampa, NULL);

    for(int k = 0; k < 0; k++){
        pthread_join(produttore[k], NULL);
    }
    
    pthread_join(master, NULL);

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&condition);

    return 0;
}

void *produci(void *arg){
    int col = (*(int *) arg);
    int sommaVettore = 0;
    for(int row = 0; row < n; row++){
        sommaVettore=sommaVettore+matrix[row][col];
    }
    pthread_mutex_lock(&mutex);
        if(sommaVettore <= valueMin){
            valueMin = sommaVettore;
        }
        indice++;
        if(indice == n){
            pthread_cond_signal(&condition);
        }
    pthread_mutex_unlock(&mutex);

    pthread_exit(NULL);
}

void *stampa(void *arg){
    pthread_mutex_lock(&mutex);
        while(indice < n){
            pthread_cond_wait(&condition, &mutex);
        }
    printf("Valore minimo->%d\n", valueMin);
    pthread_mutex_unlock(&mutex);
}

void riempiMatrice(int **matrix){
    for(int i = 0; i < n; i++){
        for(int j = 0; j < n; j++){
            matrix[i][j]=rand()%15;
        }
    }
}

void stmpMatrice(int **matrix){
    for(int i = 0; i < n; i++){
        for(int j = 0; j < n; j++){
            printf("%d\t", matrix[i][j]);
        }
    printf("\n");
    }
}