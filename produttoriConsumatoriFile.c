/* realizzare un programma in cui un thread scrittore, dato un intero N da riga di comando(13<=N<=37),scrive in un file nella prima posizione, 
uno alla volta, i numeri da 1 a N alternandosi, ogni secondo, con un thread lettore che legge, uno alla volta dalla prima posizione del file, i numeri scritti dal thread scrittore.
Un terzo thread attende la lettura dell'N-esimo intero, quindi stampa a video il messaggio "Finito, thread: tid"attende 3 secondi e termina */


#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

sem_t consumato, prodotto;
pthread_t produttori, consumatori, master;
pthread_mutex_t mutex=PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condition=PTHREAD_COND_INITIALIZER;
int n;
int indice = 0;
int fd;

void *checking(void *arg);
void *produci(void *arg);
void *consuma(void *arg);

int main(int argc, char **argv){

    if(argc < 3){
        fprintf(stderr, "Errore input!");
    }

    n = atoi(argv[1]);
    if(n < 13 && n > 37){
        fprintf(stderr, "Fuori dal rage 13-37");
    }

    if((fd=open(argv[2], O_RDWR))==-1){
        perror("Errore apertura file!");
    }

    sem_init(&consumato, 0, 1);
    sem_init(&prodotto, 0, 0);

    
    pthread_create(&master, NULL, checking, NULL);
    pthread_create(&produttori, NULL, produci, NULL);
    pthread_create(&consumatori, NULL, consuma, NULL);
    

    pthread_join(produttori, NULL);
    pthread_join(consumatori, NULL);
    pthread_join(master, NULL);

    pthread_mutex_destroy(&mutex);

    sem_destroy(&consumato);
    sem_destroy(&prodotto);
}

void *produci(void *arg){
        printf("%d", fd);
        sleep(1);
        while(1){
            sem_wait(&consumato);
                pthread_mutex_lock(&mutex);
                    if(indice == n){
                        lseek(fd,0,SEEK_SET); //Imposto l'offset per la write nella posizione 0
                        write(fd, &indice, sizeof(int));  //Scrivo l'indice nel file
                        indice++; //Incremento l'indice
                        pthread_mutex_unlock(&mutex);
                        sem_post(&prodotto);
                        break;
                    }
                pthread_mutex_unlock(&mutex); 
                
                pthread_mutex_lock(&mutex); 
                    lseek(fd,0,SEEK_SET); //Imposto l'offset per la write nella posizione 0
                    write(fd, &indice, sizeof(int)); //Scrivo l'indice nel file
                    
                    indice++;
                pthread_mutex_unlock(&mutex);
            sem_post(&prodotto);
            sleep(1);
        }
    pthread_exit(NULL);
}

void *consuma(void *arg){
        int buff;
        sleep(1);
        while(1){
            sem_wait(&prodotto);
                pthread_mutex_lock(&mutex);
                    if(indice > n){ //Se indice è maggiore di N allora ho finito
                        lseek(fd,0,SEEK_SET); //Imposto l'offset per la read nella posizione 0
                        read(fd, &buff, sizeof(int)); //Leggo l'indice nel file
                        printf("%d e indice: %d\n", buff, indice);
                        pthread_cond_signal(&condition); //Signal al thread master.
                        pthread_mutex_unlock(&mutex);
                        sem_post(&consumato);
                        break;
                    }
                pthread_mutex_unlock(&mutex);

                pthread_mutex_lock(&mutex);
                    lseek(fd,0,SEEK_SET); //Imposto l'offset per la read nella posizione 0
                    read(fd, &buff, sizeof(int)); //Leggo l'indice nel file
                    printf("%d e indice: %d\n", buff, indice);
                pthread_mutex_unlock(&mutex);
            sem_post(&consumato);
            sleep(1);
        }
    pthread_exit(NULL);
}

void *checking(void *arg){

    pthread_mutex_lock(&mutex);
    while(indice < n){
        pthread_cond_wait(&condition, &mutex);
    }
    printf("Finito thread->%ld\n", pthread_self());
    sleep(3);
    pthread_mutex_unlock(&mutex);

    pthread_exit(NULL);
    
}