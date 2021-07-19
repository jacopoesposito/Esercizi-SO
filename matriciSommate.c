/* Si realizzi un programma in C e Posix sotto Linux che, con l'ausilio della libreria Pthread, 
lancia n thread che leggono CONCORRENTEMENTE da due matrici nxn di interi allocate dinamicamente. 
Il thread i-esimo calcolerà, ogni 0.5 secondi, la somma degli elementi della colonna i-esima delle due matrici 
Un ulteriore thread attenderà fino a che tutte le entrate della nuova matrice nxn siano state calcolate e successivamente provvederà a stamparla. 
Usare le variabili di condizione. 27-06-2016 Esercizio 3*/

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

int **matriceA;
int **matriceB;
int **matriceS;
int n, indiceA; //Valore di input e indice globale

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; 
pthread_mutex_t mutexCond = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condizione = PTHREAD_COND_INITIALIZER;


void *somma(void *arg);
void *stampa(void *arg);
void riempiMatrice(int **matrix);
void stampaMatrice(int **matrix);

struct timespec tim, tim2;

int main(int argc, char **argv){

    n = atoi(argv[1]);

    indiceA = 0; 
    int count[n]; //Array per le righe

    //Inizializzo la timespec
    tim.tv_sec = 0;
    tim.tv_nsec = 500000000L;

    pthread_t lettori[n], scrittori;

    if(n%2!=0){
        printf("Devi inserire un numero pari!");
        exit(-1);
    }else if(argc > 2){
        printf("Troppi input");
        exit(-1);
    }

    matriceA = (int **)malloc(n * sizeof(int *));
    matriceB = (int **)malloc(n * sizeof(int *));
    matriceS = (int **)malloc(n * sizeof(int *));

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&condizione, NULL);


    for(int j = 0; j < n; j++){
        matriceA[j] = (int *)malloc(n * sizeof(int));
        matriceB[j] = (int *)malloc(n * sizeof(int));
        matriceS[j] = (int *)calloc(n, sizeof(int));
    }

    riempiMatrice(matriceA);
    riempiMatrice(matriceB);

    stampaMatrice(matriceA);
    printf("\n");
    stampaMatrice(matriceA);

    int i;
    for(i = 0; i < n; i++){
        count[i] = i; //Utilizzo un vettore di I = n numeri per passare l'indice della riga
        pthread_create(&lettori[i], NULL, &somma, &count[i]); //Creo N lettori che eseguono il metodo somma
    }


    pthread_create(&scrittori, NULL, &stampa, NULL); //Creo lo scrittore che stampa

    //Join e distruzione del mutex
    for(int k = 0; k < n; k++){
        pthread_join(lettori[k], NULL);
    }

    pthread_join(scrittori, NULL);
    pthread_mutex_destroy(&mutex);
    pthread_mutex_destroy(&mutexCond);
    pthread_cond_destroy(&condizione);

    return 0;
}

void *somma(void *arg){
    int i = (*(int *)arg);

    for(int j = 0; j < n; j++){
        pthread_mutex_lock(&mutex); //Mutex per la protezione della somma
        matriceS[i][j]=matriceA[i][j]+matriceB[i][j];
        pthread_mutex_unlock(&mutex);
        pthread_mutex_lock(&mutexCond); //Mutex per la protezione della variabile di condizione
        printf("THREAD %ld -> matrix[%d][%d]\n",pthread_self(),i,j);
        indiceA++; 
        nanosleep(&tim, &tim2);  
        if(indiceA == n*n){
            pthread_cond_signal(&condizione);
        }
        pthread_mutex_unlock(&mutexCond);      
    }
          
}

void *stampa(void *arg){
    pthread_mutex_lock(&mutexCond);
    while(indiceA < n*n)
        pthread_cond_wait(&condizione, &mutexCond);
            stampaMatrice(matriceS);
    pthread_mutex_unlock(&mutexCond);
}

//Inizializza la matrice passata in input con valori random
void riempiMatrice(int **matrix){
    for(int i = 0; i < n; i++){
        for(int j = 0; j < n; j++){
            matrix[i][j]=rand()%15;
        }
    }
}

//Stampa la matrice passata in input
void stampaMatrice(int **matrix){
    for(int i = 0; i < n; i++){
        for(int j = 0; j < n; j++){
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }
}


