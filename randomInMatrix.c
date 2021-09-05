/* Si realizzi un programma in C e Posix in ambiente Unix/Linux che, con l'ausilio della libreria Pthread, 
lancia n (n<10) thread che cercano di leggere una variabile il cui contenuto viene impostato ogni M secondi 
generando casualmente un numero intero compreso tra 1 e 100. 
Il thread che riesce a leggere la variabile ne scrive il contenuto in una matrice 5x5 allocata precedentemente. 
Una volta riempita la matrice, il programma termina facendo in modo che ciascun thread stampi in output 
il proprio identificativo seguito dal numero volte in cui ha letto la variabile ed uno solo di essi provvede a stampare la matrice. 
Utilizzare le variabili di condizione. */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>

#define M 5

pthread_t *consumatori;
pthread_t master;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condition = PTHREAD_COND_INITIALIZER;
sem_t prodotto;
sem_t consumato;

int *matrix;
int n;
int value = 0;
int check = 0;
int indice = 0;
int fatto = 0;

void *generate(void *arg);
void *consuma(void *arg);
int main(int argc, char **argv){

    srand(time(NULL));

    n=atoi(argv[1]);

    if(n>=10){
        fprintf(stderr, "Errore input - inserito un numero troppo grande\n");
        exit(-1);
    }

    matrix = (int *)calloc(M*M, sizeof(int));
    consumatori = (pthread_t *)malloc(n * sizeof(pthread_t));

    sem_init(&prodotto, 0, 0);
    sem_init(&consumato, 0, 1);

    for(int i = 0; i < n; i++){
        pthread_create(&consumatori[i], NULL, consuma, NULL);
    }

    pthread_create(&master, NULL, generate, NULL);

    for(int j = 0; j < n; j++){
        pthread_join(consumatori[j], NULL);
    }

    pthread_join(master, NULL);
    sem_destroy(&prodotto);
    pthread_mutex_destroy(&mutex);

    return 0;
}

void *generate(void *arg){
    while(1){
        sleep(3);
        sem_wait(&consumato);
        pthread_mutex_lock(&mutex);
        printf("Sto calcolando...\n");
            value = rand()%100;
            if(check == 1){
                pthread_mutex_unlock(&mutex);
                for(int i = 0; i < n; i++)
                sem_post(&prodotto);
                break;
            }
        pthread_mutex_unlock(&mutex);
        sem_post(&prodotto);
        //printf("Esco dal produttore\n");
    }
    
    pthread_mutex_lock(&mutex);
    while(fatto == 0)
        pthread_cond_wait(&condition, &mutex);
    for (int i = 0; i < M; i++)
    {
        for (int j = 0; j < M; j++)
        {
            printf("%d\t", *(matrix+i*M+j));
        }
        printf("\n");        
    }
    pthread_mutex_unlock(&mutex);

}

void *consuma(void *arg){ 
    int count = 0;
    while(1){
        sleep(3);      
        sem_wait(&prodotto);
        pthread_mutex_lock(&mutex);
            if(indice < M*M){                
                *(matrix+indice)=value;
                count++;
                indice++;
            }else{
                check = 1;
                pthread_mutex_unlock(&mutex);
                sem_post(&consumato);
                break;
            }
        pthread_mutex_unlock(&mutex);
        sem_post(&consumato);
    }
    pthread_mutex_lock(&mutex);
        if(fatto == 0){
            fatto = 1;
            pthread_cond_signal(&condition);
        }
    pthread_mutex_unlock(&mutex);
    printf("TID: %ld, COUNT->%d\n", pthread_self(), count);

}