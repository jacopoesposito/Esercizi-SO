/*Si realizzi un programma in C e Posix sotto Linux che, con l'ausilio della libreria pthread
lacia n (con n>1) thread produttori che scrivono concorrentemente numeri generati casualmente
nell'intervallo [1,15] ed un thread consumatore che legge i numeri prodotti. Il thread consumatore
inizia l'attività solo dopo che i produttori hanno finito di produrre. Il numero di elementi
da produrre (>100000) e il numero di thread produttori (<=30) deve essere specificato da riga
di comando. Usare mutex e variabili di condizione. 29-10-2013 Esercizio 1*/
//Nell'esercizio sono state effettuate alcune modifiche in merito al numero di produzione per facilitare i test

//Per inizializzare un mutex dinamico prima lo si dichiara come *variabile globale, 
//poi si fa la malloc di size Pthread_mutex_t, dopodichè si chiama il metodo init

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexConsuma = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t full = PTHREAD_COND_INITIALIZER;
pthread_cond_t empty = PTHREAD_COND_INITIALIZER;
int *buff;
int numProduzione;
int indexBuff = 0;

void *produci(void *arg);
void *consuma(void *arg);


int main(int argc, char **argv){
    char *numeroThread = argv[1];
    char *numeroProduzione = argv[2];

    int numThread = atoi(numeroThread);
    numProduzione = atoi(numeroProduzione);

    if(numThread > 30 || numThread <= 1){
        printf("Valore thread non rispettato!\n");
        return -1;
    }
    if(numProduzione < 10){
        printf("Valore minimo dei numeri da produrre non raggiunto\n");
        return -1;
    }

    srand( time(NULL));
    
    //Creo i vari thread
    pthread_t *produttori = (pthread_t *) malloc(numThread*sizeof(pthread_t));
    pthread_t consumatori;

    buff = (int *)calloc(numProduzione, sizeof(int));
    

    //Creo i vari mutex e semafori
    /*
    mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
    mutexConsuma = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
    full = (pthread_cond_t *) malloc(sizeof(pthread_cond_t));
    empty = (pthread_cond_t *) malloc(sizeof(pthread_cond_t));
    pthread_cond_init(full, NULL);
    pthread_cond_init(empty, NULL);
    pthread_mutex_init(mutex, NULL);
    pthread_mutex_init(mutexConsuma, NULL);
    */

    for(int i = 0; i < numThread; i++){
        pthread_create(&produttori[i], NULL, produci, NULL);
    }
    pthread_create(&consumatori, NULL, consuma, NULL);

    for(int k = 0; k < numThread; k++){
        pthread_join(produttori[k], NULL);
    }
    pthread_join(consumatori, NULL);
    



    pthread_mutex_destroy(&mutex);
    pthread_mutex_destroy(&mutexConsuma);

    return 0;
}

void *produci(void *arg){
    while(1){
        pthread_mutex_lock(&mutex);
        
        //printf("CIAO %d\n", indexBuff);  
            if(indexBuff==numProduzione){
                pthread_cond_signal(&full);
                //exit(0);
            }
            else{
                buff[indexBuff] = rand() % 15;
                printf("il numero rand e: %d\n", buff[indexBuff]);
                indexBuff++;
            }     
        pthread_mutex_unlock(&mutex);
        
    }
}

void *consuma(void *arg){
    
        pthread_mutex_trylock(&mutex);
            while(indexBuff < numProduzione){
                pthread_cond_wait(&full, &mutex);
            }
        pthread_mutex_unlock(&mutex);
        
        for(int i = 0; i < numProduzione; i++){
                printf("Buffer[%d]: %d\n", i, buff[i]);
            }
    exit(0);
}