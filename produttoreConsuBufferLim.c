/* Produttori - Consumatori con Buffer Limitato : 
 Un sistema produttori-consumatori con buffer limitati 
 si compone di un numero non specificato di thread
 produttori e consumatori ed un buffer limitato di elementi.
 Vi sono due particolati stati di questo sistema : 
 VUOTO e PIENO. 
 Lo stato PIENO del Buffer, avviene quando il produttore
 produce un nuovo elemento e lo inserisce nel buffer.
 Lo stato VUOTO del Buffer avviene quando il consumatore 
 consuma un elemento dal buffer così da creare un nuovo posto 
 per ospitare un nuovo elemento prodotto. 
 Inizialmente lo stato del Sistema sarà VUOTO. 
 Sia il Produttore che il Consumatore, rispettivamente 
 producono e consumano un elemento alla volta.
 Abbiamo le seguenti condizioni da rispettare nel problema : 
 - Un Produttore non deve sovrascrivere un Buffer Pieno. 
 - Un Consumatore non deve consumare un Buffer Vuoto.
 - I Produttori ed i Consumatori devono accedere al Buffer 
 in mutua esclusione. Opzionale : alcune volte viene 
 richiesto che l’elemento deve essere consumato nello 
 stesso ordine con il quale è stato inserito nel Buffer (FIFO) */



#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>

#define N 5

int *buffer;
int indiceBuffer = 0;
int indicePrecedente = 0;

sem_t pieno;
sem_t vuoto;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_t *produttori;
pthread_t *consumatori;

void *riempi(void *argv);
void *consuma(void *argv);

int main(int argc, char **argv){

    sem_init(&vuoto, 0, 1);
    sem_init(&pieno, 0, 0);

    buffer = (int *)calloc(4, sizeof(int));
    produttori = (pthread_t *)malloc(N*sizeof(pthread_t));
    consumatori = (pthread_t *)malloc(N*sizeof(pthread_t));

    for(int i = 0; i < N; i++){
        pthread_create(&produttori[i], NULL, riempi, NULL);
    }

    for(int j = 0; j < N; j++){
        pthread_create(&consumatori[j], NULL, consuma, NULL);
    }

    for(int n = 0; n < N; n++){
        pthread_join(consumatori[n], NULL);
    }

    for(int k = 0; k < N; k++){
        pthread_join(produttori[k], NULL);
    }

    sem_destroy(&pieno);
    sem_destroy(&vuoto);
    pthread_mutex_destroy(&mutex);

    return 0;
    }

void *riempi(void *arg){
    while(1){
        sem_wait(&vuoto);
        pthread_mutex_lock(&mutex);
        if(indiceBuffer==4){
            printf("Produttore Esce");
            pthread_mutex_unlock(&mutex);
            break;
        }
        if(buffer[indiceBuffer]!=0){
            sem_post(&pieno);
        }else{
                buffer[indiceBuffer]=1;
                printf("Prodotto: %d e indice %d\n", buffer[indiceBuffer], indiceBuffer);
                indicePrecedente=indiceBuffer;
                indiceBuffer++;
                printf("Indice produttore->%d\n", indiceBuffer);
        }
        pthread_mutex_unlock(&mutex);
        sem_post(&pieno);
    }
    pthread_exit(NULL);
}

void *consuma(void *arg){
    while(1){
        sem_wait(&pieno);
        pthread_mutex_lock(&mutex);
        if(indicePrecedente==3){
            printf("Consumatore esce\n");
            pthread_mutex_unlock(&mutex);
            sem_post(&vuoto);
            break;
        }
        if(buffer[indicePrecedente]==0){
            sem_post(&vuoto);
        }
        else{
            buffer[indicePrecedente]=0;
            printf("Consumato: %d e indice: %d\n", buffer[indicePrecedente], indicePrecedente);
            
        }
        pthread_mutex_unlock(&mutex);
        sem_post(&vuoto);
    }
    pthread_exit(NULL);
}
