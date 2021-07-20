/* In una fabbrica, N operai preparano piastrelle da far cuocere
in un forno, capace di cuocere M contemporaneamente. All'uscita
dal forno K operai visionano le piastrelle per decorarle secondo
tale sequenza di passi: se trova una piastrella difettata inizia
a prenderne dal forno 2 alla volta, altrimenti ne prende 1 alla 
volta.*/
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/types.h> 
#include <sys/stat.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
sem_t empty;
sem_t full;

int *forno;
int M, N, K, indiceA, indiceB;

void *produci(void *arg);
void *consuma(void *arg);

int main(int argc, char **argv){

    M = 20;
    N = 10;
    K = 20;
    indiceA = 0;
    indiceB = 0;
    //srand(time(NULL));

    //Utilizzo semafori per nome
    sem_init(&empty, 0, 1);
    sem_init(&full, 0, 0);


    //Inizializzo i thread con la malloc
    pthread_t *operaiConsumatori = (pthread_t *)malloc(K * sizeof(pthread_t));
    pthread_t *operaiProduttori = (pthread_t *)malloc(M * sizeof(pthread_t));

    //inizializzo l'array forno con la calloc
    forno = (int *) calloc(N, sizeof(int));

    

    for(int i = 0; i < M; i++){
        pthread_create(&operaiProduttori[i], NULL, produci, NULL); //Creo i thread produttori
    }

    for(int j = 0; j < K; j++){
        pthread_create(&operaiConsumatori[j], NULL, consuma, NULL); //Creo i thread consumatori
    }

    for(int k = 0; k < M; k++){
        pthread_join(operaiProduttori[k], NULL); //Joino i thread produttori
    }

    for(int n = 0; n < K; n++){
        pthread_join(operaiConsumatori[n], NULL); //joino i thread consumatori
    }

    //Stampo l'array forno dopo che i produttori e i consumatori hanno finito di lavorare
    for(int m = 0; m < N; m++){
        printf("Piastrella: %d ID:%d\n", forno[m], m);
    }

    pthread_mutex_destroy(&mutex); //distruggo il mutex
    sem_destroy(&empty); //distruggo i semafori
    sem_destroy(&full);
    free(forno);  

    return 0;
}

void *produci(void *arg){
    sem_wait(&empty);  //Se vuoto aspetta che sia riempito, all'inzio il semaforo binario è inizializzato a 1 quindi passa la wait
        if(forno[indiceA]==0 && indiceA < 10){
            pthread_mutex_lock(&mutex); //Blocco il mutex e accedo alla sezione critica
            int r = (int)(1.0 + 2.0 * rand() / RAND_MAX); //Genero int randomico compreso fra 1 e 2, 1 rappresenta una piastrella buona, due una rotta
            forno[indiceA]=r;
            //printf("%d\n", r);
            //printf("%d %d\n", forno[indiceA], indiceA);
            indiceA++;
            pthread_mutex_unlock(&mutex); 
        }
    sem_post(&full); //Faccio la post su full
}

void *consuma(void *arg){
    sem_wait(&full);
        if(forno[indiceB]==1 && indiceB < 10){
            pthread_mutex_lock(&mutex);
            forno[indiceB]=0;
            //printf("%d %d\n", forno[indiceB], indiceB);
            indiceB++;
            pthread_mutex_unlock(&mutex);
        }
        else if(forno[indiceB]==2 && indiceB < 10){
            pthread_mutex_lock(&mutex);
            forno[indiceB]=0;
            //printf("%d %d\n", forno[indiceB], indiceB);
            if(indiceB < 9){
            indiceB++;
            forno[indiceB]=0;
            //printf("%d %d\n", forno[indiceB], indiceB);
            }
            pthread_mutex_unlock(&mutex);
        }
    sem_post(&empty);
}
