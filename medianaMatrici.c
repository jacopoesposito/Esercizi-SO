/* Si realizzi un programma in C e Posix sotto Linux che, con l’ausilio della libreria Pthread, lancia n thread
per cercare la mediana di valore massimo tra le n righe di una matrice nxn di interi. Individuato il valore
cercato, si provvederà a stamparlo a video. La dimensione della matrice può essere fornita in input al
programma in fase di esecuzione o da riga di comando */


#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

int n;
int **matrix;
int valueMax = 0;
pthread_t *calcolatori;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *findMedianaMax(void *arg);
void stampaMatrice(int **matrix);
void riempiMatrici(int **matrix);

int main(int argc, char **argv){
    
    srand(time(NULL));

    if(argc < 2){
        scanf("%d", &n);
    }
    else{
        n = atoi(argv[1]);
    }

    int count[n];
    
    calcolatori = (pthread_t *)malloc(n*sizeof(pthread_t));
    matrix = (int **)malloc(n * sizeof(int *));

    for(int k = 0; k < n; k++){
        matrix[k] = (int *)malloc(n * sizeof(int));
    }

    riempiMatrici(matrix);

    for(int i = 0; i < n; i++){
        count[i] = i;
        pthread_create(&calcolatori[i], NULL, findMedianaMax, &count[i]);
    }

    for(int j = 0; j < n; j++){
        pthread_join(calcolatori[j], NULL);
    }

    stampaMatrice(matrix);
    printf("\n Il valore della mediana massima è -> %d\n", valueMax);

    pthread_mutex_destroy(&mutex);
}

void *findMedianaMax(void *arg){
    int i = (*(int *)arg);
    int col = (n-1)/2;
    int valoreMediana = matrix[i][col];
    pthread_mutex_lock(&mutex);
        if(valoreMediana > valueMax){
            valueMax = valoreMediana;
        }
    pthread_mutex_unlock(&mutex);
    
    pthread_exit(NULL);
}


void riempiMatrici(int **matrix){
    for(int i = 0; i < n; i++){
        for(int j = 0; j < n; j++){
            matrix[i][j]=rand()%20;
        }
    }
}

void stampaMatrice(int **matrix){
    for(int i = 0; i < n; i++){
        for(int j = 0; j < n; j++){
            printf("%d\t ", matrix[i][j]);
        }
        printf("\n");
    }
}