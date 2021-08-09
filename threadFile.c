/*Si realizzi un programma in C e Posix sotto Linux che, con l’ausilio della libreria Pthread, lancia 3 thread
per cercare il carattere x rispettivamente nei file primo.txt, secondo.txt e terzo.txt.  Ciascun thread 
provvede ad aggiornare una variabile condivisa, totale, che tiene traccia di ogni occorrenza del carattere
x nel file.*/


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


pthread_t *lavoratori;
pthread_mutex_t mutex;
char carattere;
int fd[3];
int count = 0;

void *lavora(void *arg);

int main(int argc, char **argv){
    
    if(argc < 2){
        fprintf(stderr, "Errore input");
    }
    
    carattere = argv[1][0];

    if((fd[0]=open(argv[2], O_RDONLY))==-1){
        perror("Errore apertura file 1");
    }

    if((fd[1]=open(argv[3], O_RDONLY))==-1){
        perror("Errore apertura file 2");
    }

    if((fd[2]=open(argv[4], O_RDONLY))==-1){
        perror("Errore apertura file 3");
    }

    lavoratori = (pthread_t *)malloc(3 * sizeof(pthread_t));

    for(int i = 0; i < 3; i++){
        pthread_create(&lavoratori[i], NULL, lavora, &fd[i]);
    }

    for(int j = 0; j < 3; j++){
        pthread_join(lavoratori[j], NULL);
    }

    printf("Numero occorenze->%d", count);

    close(fd[0]);
    close(fd[1]);
    close(fd[2]);

    pthread_mutex_destroy(&mutex);

    return 0;   
}

void *lavora(void *arg){
    struct stat fileStat;
    int fd = (*(int *) arg);
    if((fstat(fd, &fileStat))<0){
        perror("Errore fstat");
    }
    char bufferFile[fileStat.st_size];
    read(fd, bufferFile, sizeof(bufferFile));
    for(int i = 0; i < sizeof(bufferFile); i++){
        if(bufferFile[i]==carattere){
            pthread_mutex_lock(&mutex);
            count++;
            pthread_mutex_unlock(&mutex);
        }
    }
    pthread_exit(NULL);
}