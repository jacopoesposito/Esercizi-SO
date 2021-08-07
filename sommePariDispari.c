/*  Si realizzi un programma in C e Posix sotto Linux che, con l'ausilio della libreria Pthread effettui i seguenti task: 
    lancia n thread per calcolare la somma degli elementi di ciascuna riga di una matrice n x n di interi generati casualmente in un intervallo [0,255], allocata dinamicamente
    Il calcolo della somma degli elementi di ciascuna riga deve essere effettuato concorrentemente su tutte le righe, secondo la seguente modalità:
    i) il thread i-esimo con i pari, calcola la somma degli elementi di indice pari della riga i-esima
    ii) il thread con indice i dispari, calcola la somma degli elementi di indice dispari della riga i-esima. 
    Calcolate le somme parziali, si provvederà a ricercarne il minimo ed a stamparlo a video. 

    Created by: SamSan-Tech
*/


#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <limits.h>
#include <pthread.h>

pthread_t *calcolatori;
pthread_mutex_t mutex=PTHREAD_MUTEX_INITIALIZER;
int minimo = INT_MAX;
int **matrix;
int n;
int indice;
int *array;

void *somma(void *arg);
void riempiMatrice(int **matrix);
void stmpMatrice(int **matrix);

int main(int argc, char **argv){

    if(argc < 2){
       fprintf(stderr, "Errore Input"); 
    }
    
    n = atoi(argv[1]);
    
    matrix=(int **)malloc(n * sizeof(int *));
    calcolatori=(pthread_t *)malloc(n * sizeof(pthread_t));
    array=(int *)malloc(n * sizeof(int));

    for(int i = 0; i < n; i++){
        matrix[i]=(int *)malloc(n * sizeof(int));
    }

    riempiMatrice(matrix);
    stmpMatrice(matrix);

    int count[n];
    for(int j = 0; j < n; j++){
        count[j]=j;
        pthread_create(&calcolatori[j], NULL, somma, &count[j]);
    }

    for(int k = 0; k < n; k++){
        pthread_join(calcolatori[k], NULL);
    }
    
    for(int e = 0; e < n; e++){
        printf("Array ->%d\n", array[e]);
    }

    for(int v = 0; v < n; v++){
        if(array[v]<minimo){
            minimo=array[v];
        }
    }    

    printf("Il minimo è: %d (๑˃ᴗ˂)ﻭ\n", minimo);

    return 0;
}

void *somma(void *arg){
    int somma  = 0;
    int row = (*(int *) arg);
    if((n%2)==0){
        if((row%2)==0){
            for(int i = 0; i < n-1; i=i+2){
                somma+=matrix[row][i];         
            }
            array[row]=somma;
        }
        else{
            for(int j = 1; j < n; j=j+2){
                somma+=matrix[row][j];
            }
            array[row]=somma;
        }
    }else{
        if((row%2)!=0){
            for(int i = 0; i < n; i=i+2){
                somma+=matrix[row][i];
            }
            array[row]=somma;
        }
        else{
            for(int j = 1; j < n-1; j=j+2){
                somma+=matrix[row][j];
            }
            array[row]=somma;
        }
    }
    pthread_exit(NULL);    
    
}


void riempiMatrice(int **matrix){
    for(int i = 0; i < n; i++){
        for(int j = 0; j < n; j++){
            matrix[i][j]=rand()%255;
        }
    }
}

void stmpMatrice(int **matrix){
    for(int i = 0; i < n; i++){
        for(int j = 0; j < n; j++){
            printf("%d\t", matrix[i][j]);
        }
        printf("\n");
    }
}