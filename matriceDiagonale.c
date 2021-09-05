/* Si realizzi un programma in C e Posix sotto Linux che, con l'ausilio della libreria Pthread, lancia n thread per calcolare il valore massimo degli elementi delle diagonali principali di un insieme di n matrici di dimensione 3x3. Ciascuna matrice contiene interi generati casualmente in un intervallo [0,100]. Il calcolo dei massimi degli elementi di ciascuna diagonale per ciascuna matrice deve essere effettuato concorrentemente. Il numero di matrici deve essere fornito in input al programma da riga di comando. Si utilizzi un semaforo binario Poisx. */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <semaphore.h>
#include <unistd.h>
#include <pthread.h>

int n;
typedef struct matrici{
	int **M;
}matriciN;

matriciN *matrix;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
sem_t massimo;

int max=0;
int cont=0;


void creaMatrici(int n);

void *max_diagonale_matrice(void *arg);

int main(int argc, char **argv){
	
	srand(time(NULL));
	n=atoi(argv[1]);

	if(n < 2){
		fprintf(stderr, "Errore input");
	}

	int count[n];
	pthread_t lavoratori[n];

	sem_init(&massimo,0,0);	
	
	printf("non faccio niente\n");
	
	creaMatrici(n);
	
	

	for(int i=0;i<n;i++){
		count[i]=i;
		pthread_create(&lavoratori[i],NULL,max_diagonale_matrice,&count[i]);	
	}


	for(int i=0;i<n;i++){
		pthread_join(lavoratori[i],NULL);
	}
	
	pthread_mutex_destroy(&mutex);

	return 0;
}

void *max_diagonale_matrice(void *arg){
	int indice=(*(int*)arg);

	
	for(int i=0;i<3;i++){
		pthread_mutex_lock(&mutex);
		if((matrix+indice)->M[i][i]>max)
			max=(matrix+indice)->M[i][i];
		cont++;
		pthread_mutex_unlock(&mutex);
	}

	if(cont==3){
		sem_wait(&massimo);
		printf("Il massimo e': %d\n",max);
	}
	
	if(cont==(n*3)){
		sem_post(&massimo);
		
	}
	
	pthread_exit(NULL);

}

void creaMatrici(int n){
	
	matrix=(matriciN*)malloc(n*sizeof(matriciN));

	for(int i=0;i<n;i++){
		(matrix+i)->M=(int**)malloc(3*sizeof(int*));
		for(int j=0;j<3;j++){
			(matrix+i)->M[j]=(int *)malloc(3*sizeof(int*));		
		}
			
	}
	
	
	for(int k=0;k<n;k++){
		printf("Matrice %d:\n",k+1);
		for(int i=0;i<3;i++){
			for(int j=0;j<3;j++){
				(matrix+k)->M[i][j]=rand()%100+1;
				printf("%d\t",(matrix+k)->M[i][j]);
			}
			printf("\n");	
		}
		printf("\n\n");	
	}
	
	
}




