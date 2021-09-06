/* Scrivere un programma C in ambiente Linux che facendo uso della libreria Pthread realizzi il seguente
comportamento: un master thread inizializza una variabile globale a zero, crea un thread produttore ed
un thread consumatore e, in un ciclo infinito, stampa il valore della variabile globale. Il produttore
incrementa, ad ogni passo, la variabile globale di due unità e dorme per un secondo. Il consumatore
decrementa la variabile globale di una unità e dorme per un secondo. Il processo termina quando la
variabile globale è maggiore di un valore intero passato da riga di comando. Utilizzare un semaforo Posix
per la sincronizzazione */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>

int n;
int value;

pthread_t produttori;
pthread_t consumatori;
pthread_t master;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
sem_t prodotto;
sem_t consumato;
sem_t fatto;

void *checking(void *arg);
void *produci(void *arg);
void *consuma(void *arg);

int main(int argc, char **argv){
	
	if(argc < 2){
		printf("Errore Input!");
	}

	n = atoi(argv[1]);

	sem_init(&prodotto, 0, 0);
	sem_init(&consumato, 0, 1);
	sem_init(&fatto, 0, 0);

	pthread_create(&master, NULL, checking, NULL);
	
	pthread_join(master, NULL);

	sem_destroy(&prodotto);
	sem_destroy(&consumato);

	pthread_mutex_destroy(&mutex);

	return 0;
}

void *checking(void *arg){
	
	value = 0;

	pthread_create(&produttori, NULL, produci, NULL);
	pthread_create(&consumatori, NULL, consuma, NULL);
	
	
	while(1){
		sem_wait(&fatto);
		pthread_mutex_lock(&mutex);
		if(value>n){
			pthread_mutex_unlock(&mutex);
			break;
		}
		printf("%d\n", value); 
		pthread_mutex_unlock(&mutex);
	}
	
	pthread_join(produttori, NULL);
	pthread_join(consumatori, NULL);
}

void *produci(void *arg){
	while(1){
		sem_wait(&consumato);
		pthread_mutex_lock(&mutex);
			if(value>n){
				pthread_mutex_unlock(&mutex);
				sem_post(&prodotto);
				sem_post(&fatto);
				break;
			}
		value+=2;
		pthread_mutex_unlock(&mutex);
		sem_post(&fatto);
		sleep(1);
		sem_post(&prodotto);
		//printf("Esco dal produttore\n");
	
	}
}

void *consuma(void *arg){
	while(1){
		sem_wait(&prodotto);
		pthread_mutex_lock(&mutex);
			if(value>n){
				pthread_mutex_unlock(&mutex);
				sem_post(&consumato);
				sem_post(&fatto);
				break;
			}
			value-=1;
		pthread_mutex_unlock(&mutex);
		sem_post(&fatto);
		sleep(1);
		sem_post(&consumato);
		//printf("Esco dal consumatore\n");
			
	}
}
