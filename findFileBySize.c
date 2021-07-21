/* Si realizzi un programma in C e POSIX sotto Linux che presi due parametri da riga di
comando (Il primo parametro sarà il percorso di una directory, e il secondo parametro sarà
un numero intero che rappresenta la taglia di un file) faccia il parsing della directory 
passato come primo argomento e stampi i nomi e le grandezze dei contenuti in tale directory
per cui la taglia di ciascun file sia maggiore del valore fornito come secondo parametro. */


#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(int argc, char **argv){
    DIR *dir;
    struct dirent *fileMin;
    int size;

    if(argc > 3){
        printf("Errore input programma");
    }
    dir = opendir(argv[1]);
    size = atoi(argv[2]);

    if(dir==NULL){
        printf("La directory inserita non esiste!");
    }

    while((fileMin = readdir(dir))!=NULL){
        if(fileMin->d_reclen>=size){
            printf("Nome: %s, Size: %d\n", fileMin->d_name, fileMin->d_reclen);
            
        }
    }
}