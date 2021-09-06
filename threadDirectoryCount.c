/*Si realizzi un programma in C e Posix sotto Linux che, con l’ausilio della libreria Pthread, lancia 3 thread
che leggono concorrentemente altrettante directory specificate dall'utente. I thread, inoltre, scrivono in
una variabile il numero di file della directory, tra le tre esaminate, contenente il minor numero di entrate.*/



#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <dirent.h>
#include <limits.h>

int valueMin=INT_MAX;

typedef struct directory{
	char *dir;
}directoryFiles;

directoryFiles cartelle[3];

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_t *lettori;

void *leggi(void *arg);

int main(int argc, char **argv){

	if(argc < 4){
		fprintf(stderr, "Errore input!");
	}

	cartelle[0].dir = argv[1];
	cartelle[1].dir = argv[2];
	cartelle[2].dir = argv[3];
	
	printf("%s\n", cartelle[0].dir);
	lettori = (pthread_t *)malloc(3 * sizeof(pthread_t));

	for(int i = 0; i < 3; i++){
		pthread_create(&lettori[i], NULL, leggi, cartelle[i].dir);
	}
	
	for(int j = 0; j < 3; j++){
		pthread_join(lettori[j], NULL);	
	}

	pthread_mutex_destroy(&mutex);
	
	printf("Il valore minimo di record è: %d", valueMin);

	return 0;
}

void *leggi(void *arg){
	struct dirent *file;
	DIR *dir;
	char *dirThread;
	dirThread = (char *) arg;
	int count;

	if((dir = opendir(dirThread))==NULL){
		printf("Errore apertura directory: %s",dirThread);
		pthread_exit(NULL);
	}

	while((file = readdir(dir))!=NULL){
		count++;	
	}
	
	printf("Record nella cartella: %d\n", count);
	
	pthread_mutex_lock(&mutex);
		if(count<valueMin){
			valueMin = count;
		}
	pthread_mutex_unlock(&mutex);
	
	pthread_exit(NULL);
}
