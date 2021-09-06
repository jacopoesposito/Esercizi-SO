/* Si realizzi un programma C e Posix in ambiente Linux che, impiegando la libreria Pthread, crei una matrice di interi di dimensioni n x n con n numero dispari fornito da riga di comando, che provveda a sommare in modo concorrente con quattro thread, gli elementi delle due diagonali, della riga centrale e della colonna centrale della matrice e ne determini il massimo da assegnare ad un'opportuna variabile.  */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <time.h>
#include <pthread.h>

int n;
int **matrix;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int massimo=0;

void* somma_diagonale1(void* arg);
void* somma_diagonale2(void* arg);
void* somma_riga_centrale(void* arg);
void* somma_colonna_centrale(void* arg);



int main(int argc, char **argv){
	
	srand(time(NULL));

	if(argc<2 || argc>2){
		fprintf(stderr,"Errore input: inserire un numero dispari da riga di comando\n");
		exit(-1);
	}

	n=atoi(argv[1]);

	if(n%2==0){
		fprintf(stderr,"Errore input: inserire numero dispari\n");
		exit(-2);
	}
	
	matrix=(int**)malloc(n*sizeof(int*));
	for(int i=0;i<n;i++)
		matrix[i]=(int*)calloc(n,sizeof(int));
	
	for(int i=0;i<n;i++){
		for(int j=0;j<n;j++){
			matrix[i][j]=rand()%10+1;
			printf("%d\t",matrix[i][j]);
		}
		printf("\n");
	}

	pthread_t sommatore[4];
	

	pthread_create(&sommatore[0],NULL,somma_diagonale1,NULL);
	pthread_create(&sommatore[1],NULL,somma_diagonale2,NULL);
	pthread_create(&sommatore[2],NULL,somma_riga_centrale,NULL);
	pthread_create(&sommatore[3],NULL,somma_colonna_centrale,NULL);

	for(int i=0;i<4;i++){
		pthread_join(sommatore[i],NULL);
	}
	
	pthread_mutex_destroy(&mutex);

	printf("il massimo e': %d\n",massimo);

	return 0;
}


void *somma_diagonale1(void* arg){
	int somma=0;
	for(int i=0;i<n;i++){
		somma+=matrix[i][i];
	}
	
	pthread_mutex_lock(&mutex);
	if(massimo<somma)
		massimo=somma;
	pthread_mutex_unlock(&mutex);

	printf("Il massimo qui e': %d\n",somma);
	

	pthread_exit(NULL);
	
}

void *somma_diagonale2(void* arg){
	
	int somma=0;

	for(int i=0;i<n;i++)
		somma+=matrix[n-1-i][i];

	pthread_mutex_lock(&mutex);
	if(massimo<somma)
		massimo=somma;
	pthread_mutex_unlock(&mutex);

	printf("Il massimo qui e': %d\n",somma);

	pthread_exit(NULL);
}

void *somma_riga_centrale(void* arg){
	
	int somma=0;
	
	for(int i=0;i<n;i++)
		somma+=matrix[n/2][i];

	pthread_mutex_lock(&mutex);
	if(massimo<somma)
		massimo=somma;
	pthread_mutex_unlock(&mutex);
printf("Il massimo qui e': %d\n",somma);

	pthread_exit(NULL);	
}

void *somma_colonna_centrale(void* arg){
	
	int somma=0;
	
	for(int i=0;i<n;i++)
		somma+=matrix[i][n/2];

	pthread_mutex_lock(&mutex);
	if(massimo<somma)
		massimo=somma;
	pthread_mutex_unlock(&mutex);
printf("Il massimo qui e': %d\n",somma);

	pthread_exit(NULL);
	
}






