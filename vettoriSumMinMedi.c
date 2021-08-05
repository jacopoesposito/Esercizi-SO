/*
    Si realizzi un programma C e Posix in ambiente Linux che, impiegando la libreria Pthread, 
    crei un vettore di interi di dimensione n, dove n>=24 e multiplo di 4, è un argomento passato da riga di comando. 
    Calcolati i valori medi degli elementi contenuti nei quattro sottovettori di taglia n/4, il programma deve determinare, 
    impiegando 4 thread, la somma del numero di elementi minori del valore medio di ciascun sottovettore.
    la somma di tutti gli elementi appartenenti al pezzo di vettore diviso numero elementi pezzo vettore
*/

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <limits.h>

int n;
int *array;
int valoreMedio[4];
pthread_t *sommatori;

typedef struct{
    int i;
    int value;
}dettagliVettore;

void *somma(void *arg);
void calcolaValoreMedio(int valoreMedio, int index);

int main(int argc, char **argv){

    dettagliVettore dVettore[4];

    if(argc < 2){
        fprintf(stderr, "ERRORE INPUT!\n");
        exit(-1);
    }
    
    n = atoi(argv[1]);

    if(n < 24 && (n%4)!=0){
        fprintf(stderr, "ERRORE INPUT! INSERIRE UN NUMERO >= 24 E MULTIPLO DI 4\n");
        exit(-2);
    }

    array = (int *)malloc(n*sizeof(int));

    srand(time(NULL));
    for(int i = 0; i < n; i++){
        array[i]=rand()%20;
    }

    int count[4];

    for(int e = 0; e < 4; e++){
        calcolaValoreMedio(valoreMedio[e], (n/4)*e);
    }

    sommatori = (pthread_t *)malloc(4*sizeof(pthread_t));
    for(int j = 0; j < 4; j++){
        dVettore[j].i=(n/4)*j;
        dVettore[j].value=valoreMedio[j];
        pthread_create(&sommatori[j], NULL, somma, &dVettore[j]);
    }

    for(int k = 0; k < 4; k++){
        pthread_join(sommatori[k], NULL);
    }


    return 0;

}

void *somma(void *arg){
    int valueMin = INT_MAX;
    int somma = 0;
    dettagliVettore s = (*(dettagliVettore *)arg);
    for(int i = s.i; i < ((n/4)+s.i); i++){
        if(s.value < array[i]){
            somma+=array[i];
        }        
    }
    printf("Il thread #%ld ha sommato i valori minimi:%d\n", pthread_self(), somma);
}

void calcolaValoreMedio(int valoreMedio, int index){
    int totale = 0;
    for(int i = index; i < ((n/4)+index); i++){
        totale+=array[index];
    }
    valoreMedio=totale/(n/4);
}