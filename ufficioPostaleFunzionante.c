/*Un ufficio postale eroga due tipo di servizi A e B.
Nell'ufficio sono aperti N sportelli per il servizio
A e M sportelli per il servizio B.
All'arrivo, un cliente prende un numero per il 
servizio richiesto ed attende il suo turno.
Ogni impiegato dopo aver servito un cliente,
chiama il numero successivo per il servizio erogato dal proprio sportello. 
Nel caso in cui non ci siano clienti l'Impiegato si 
mette in attesa e viene risvegliato dall'arrivo
di un nuovo cliente. L'ufficio non accetta 
prenotazioni per il servizio A dopo l'ora X e per
il servizio B dopo l'ora Y.*/


#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

#define N 10
#define M 10
#define X 12
#define Y 12

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
sem_t arrivoA;
sem_t arrivoB;
sem_t codaA;
sem_t codaB;
sem_t prendiNumero;

int filaA[N];
int filaB[M];
pthread_t *clienti;
int indice;
int indiceA;
int indiceB;
int *clientiServiti;

int hA=0;

void *cliente(void *arg);
void *sportelloA(void *arg);
void *sportelloB(void *arg);

int main(int argc, char **argv){
    indice = 0;
    indiceA = 0;
    indiceB = 0;
    srand(time(NULL));

    char *a = argv[1];
    int numeroClienti = atoi(a);


    //Inizializzo semafori
    sem_init(&arrivoA, 0, 0);
    sem_init(&arrivoB, 0, 0);
    sem_init(&codaA, 0, N);
    sem_init(&codaB, 0, M);
    sem_init(&prendiNumero, 0, 1);


    //Inizializzo thread
    pthread_t *sportelliA = (pthread_t *) malloc(M*sizeof(pthread_t));
    pthread_t *sportelliB = (pthread_t *) malloc(N*sizeof(pthread_t));
    pthread_t *clienti = (pthread_t *) malloc(numeroClienti*sizeof(pthread_t));

    clientiServiti = (int *) calloc(numeroClienti, sizeof(int));

    for (int y = 0; y < numeroClienti; y++){
        printf("%d", clientiServiti[y]);
    }


    //Creo thread
    for(int i = 0; i < M; i++){
        pthread_create(&sportelliA[i], NULL, sportelloA, NULL);
    }

    for(int i = 0; i < N; i++){
        pthread_create(&sportelliB[i], NULL, sportelloB, NULL);
    }

    for(int i = 0; i < numeroClienti; i++){
        pthread_create(&clienti[i], NULL, cliente, NULL);
    }

    
    for(int j = 0; j < numeroClienti; j++){
        pthread_join(clienti[j], NULL);
    }

    for(int n = 0; n < N; n++){
        pthread_join(sportelliA[n], NULL);
    }

    for(int t = 0; t < N; t++){
        pthread_join(sportelliB[t], NULL);
    }


   

    pthread_mutex_destroy(&mutex);
    sem_destroy(&arrivoA);
    sem_destroy(&arrivoB);
    sem_destroy(&codaA);
    sem_destroy(&codaB);
    sem_destroy(&prendiNumero);
    free(clientiServiti);


    return 0;
}

void *cliente(void *arg){
    int scelta = rand() % 2;
    if(scelta == 0){
        printf("Sono nella scelta 0 sto per fare la wait\n");
        sem_wait(&codaA);
        printf("Ho fatto la wait\n");
        pthread_mutex_lock(&mutex);
        printf("Ciao sto per accedere all'array\n");
        clientiServiti[indice]=1;
        indice++;
        pthread_mutex_unlock(&mutex);
        printf("Post su A\n");
        sem_post(&arrivoA);
    }
    else{
        printf("Sono nella scelta 1 sto per fare la wait\n");
        sem_wait(&codaB);
        printf("Ho fatto la wait\n");
        pthread_mutex_lock(&mutex);
        printf("Ciao sto per accedere all'array\n");
        clientiServiti[indice]=1;
        indice++;
        pthread_mutex_unlock(&mutex);
        printf("Post su B\n");
        sem_post(&arrivoB);
    }
}

void *sportelloA(void *arg){
        sem_wait(&arrivoA);
        if(clientiServiti[indiceA]!=0){
            pthread_mutex_lock(&mutex);
            printf("%d\n", clientiServiti[indiceA]);
            indiceA++;
            printf("Lo sportello A ha servito\n");
            pthread_mutex_unlock(&mutex);
            sem_post(&codaA);
        }
        else sem_post(&codaA); 
}

void *sportelloB(void *arg){
        sem_wait(&arrivoB);
        if(clientiServiti[indiceB]!=0){
            pthread_mutex_lock(&mutex);
            printf("%d\n", clientiServiti[indiceB]);
            indiceB++;
            printf("Lo sportello B ha servito\n");
            pthread_mutex_unlock(&mutex);
            sem_post(&codaB);
        }
        else sem_post(&codaB);       
}