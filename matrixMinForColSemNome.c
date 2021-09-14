/*
    Si realizzi un programma in C e Posix in ambiente Linux che, impiegando la libreria Pthread, 
    creai una matrice di interi di dimensione nxn con n passato come argomento da riga di comando, 
    e provveda a sommare concorrentemente ciascuna colonne della matrice e a determinarne il minimo 
    il cui valore dev'essere assegnato ad una variabile globale minimo e poi al termine stampato da un altro thread.

	Variante che utilizza un semaforo per nome per svegliare il master.
*/


#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>
#include <time.h>
#include <limits.h>
#include <fcntl.h> 
#include <sys/stat.h> 

int n;
int **matrix;
int minimo = INT_MAX;
int indice = 0;

pthread_t *calcolatori, master;
sem_t *fatto;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void riempiMatrice(int **matrix);
void stmpMatrice(int **matrix);

void *calcola(void *arg);
void *checking(void *arg);

int main(int argc, char **argv){
	
	srand(time(NULL));

	sem_unlink("/fatto");
	
	fatto = sem_open("/fatto", O_CREAT, 0777, 0);

	if(argc < 2){
		fprintf(stderr, "Errore input");
		exit(-1);
	}

	n = atoi(argv[1]);
	matrix = (int **)malloc(n * sizeof(int *));
	calcolatori = (pthread_t *)malloc(n * sizeof(pthread_t));

	for(int i = 0; i < n; i++){
		matrix[i]= (int *)malloc(n * sizeof(int));
	}
	
	riempiMatrice(matrix);
	stmpMatrice(matrix);
	
	pthread_create(&master, NULL, checking, NULL);
	
	int count[n];
	for(int j = 0; j < n; j++){
		count[j] = j;
		pthread_create(&calcolatori[j], NULL, calcola, &count[j]);
	}

	

	for(int i = 0; i < n; i++){
		pthread_join(calcolatori[i], NULL);
	}

	pthread_join(master, NULL);

	sem_destroy(fatto);

	pthread_mutex_destroy(&mutex);

	return 0;

}

void *calcola(void *arg){
	int somma = 0;
	int col = (*(int *) arg);

	for(int i = 0; i < n; i++){
		somma += matrix[i][col];
	}

	pthread_mutex_lock(&mutex);
	if(somma<minimo){
		minimo = somma;
	}
	indice++;
	write(0, &indice, sizeof(int));
	if(indice == n){
		sem_post(fatto);
	}
	pthread_mutex_unlock(&mutex);

	pthread_exit(NULL);

}

void *checking(void *arg){
	sem_wait(fatto);
	printf("Il valore minimo è: %d", minimo);

	pthread_exit(NULL);

}

void stmpMatrice(int **matrix){
	for(int i = 0; i < n; i++){
		for(int j = 0; j < n; j++){
			printf("%d\t", matrix[i][j]);
		}
	printf("\n");
	}

}

void riempiMatrice(int **matrix){

	for(int i = 0; i < n; i++){
		for(int j = 0; j < n; j++){
			matrix[i][j]=rand()%15;
		}
	}


}
