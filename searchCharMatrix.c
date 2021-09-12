/* 
    Si realizzi un programma in C e Posix che, utilizzando la libreria Pthread, lancia n thread per cercare un elemento in una matrice nxn di caratteri.
    Ognuno dei thread cerca l'elemento in una delle righe della matrice.
    Non appena un thread ha trovato l'elemento cercato, rende note agli altri thread le coordinate dell'elemento e tutti i thread terminano.
*/


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

char **matrix;
char elemento;
int n;
int check=0;
int indice=0;
pthread_t *cercatori;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condition = PTHREAD_COND_INITIALIZER;

void riempiMatrice(char **matrix);
void stmpMatrice(char **matrix);
void *cerca(void *arg);

int main(int argc, char **argv){
    srand(time(NULL));

    if(argc < 3){
        fprintf(stderr, "ERRORE INPUT!");
    }

    n = atoi(argv[1]);
    elemento = argv[2][0];

    matrix = (char **)malloc(n * sizeof(char *));
    cercatori = (pthread_t *)malloc(n * sizeof(pthread_t));

    for(int i = 0; i < n; i++){
        matrix[i]=(char *)malloc(n * sizeof(char));
    }
    riempiMatrice(matrix);
    stmpMatrice(matrix);
    int count[n];

    for(int k = 0; k < n; k++){
        count[k] = k;
        pthread_create(&cercatori[k], NULL, cerca, &count[k]);
    }

    for(int j = 0; j < n; j++){
        pthread_join(cercatori[j], NULL);
    }

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&condition);

}

void *cerca(void *arg){

    int row = (*(int *) arg);
    pthread_mutex_lock(&mutex); //Inserisco un unico mutex perchè i controlli da fare all'interno del for sono molti, e non è conveniente aprire e chiudere il mutex
        for(int col = 0; col < n; col++){
            if(check==1){ //Se check è uguale ad uno l'elemento è stato trovato e quindi l'iesimo thread esce dal ciclo e aspetta la signal
                break;
            }
            indice++;
            if(matrix[row][col]==elemento){ //Se trovo l'elemento lo comunico all'utente, setto check a 1 ed eseguo la signal ai thread in attesa
                fprintf(stdout, "Elemento Trovato! Posizione->[%d][%d]\n", row, col);
                check = 1;
                for(int i = 0; i < n; i++)
                    pthread_cond_signal(&condition);
            }
            if(indice==n*n && matrix[row][col]!=elemento){ //Se l'indice è uguale alla grandezza della matrice allora nessun elemento è stato trovato, setto check a 1 e faccio la signal
                fprintf(stdout, "NESSUNA CORRISPONDENZA TROVATA, 你好!\n");
                check = 1;
		pthread_mutex_unlock(&mutex);
                for(int j = 0; j < n; j++)
                    pthread_cond_signal(&condition);
            }
        }
    pthread_mutex_unlock(&mutex);

    pthread_mutex_lock(&mutex);
        while(check==0) //Se non ho trovato elementi mi metto in attesa della signal
            pthread_cond_wait(&condition, &mutex);
        printf("THREAD #%d ESCE\n", pthread_self());
    pthread_mutex_unlock(&mutex);
    pthread_exit(NULL);
}

void riempiMatrice(char **matrix){
    for(int i = 0; i < n; i++){
        for(int j = 0; j < n; j++){
            matrix[i][j]=rand()%26+97;
        }
    }
}

void stmpMatrice(char **matrix){
    for(int i = 0; i < n; i++){
        for(int j = 0; j < n; j++){
            printf("%c\t", matrix[i][j]);
        }
        printf("\n");
    }
}
