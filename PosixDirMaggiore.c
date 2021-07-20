/* Scrivere un programma C e POSIX sotto Linux che, presa in input una cartella, 
dopo averne analizzato il suo contenuto, identifichi il file di lunghezza massima 
e ne stampi la maschera dei permessi relativi agli altri Traccia 22-06-2021 Esercizio 4*/

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <limits.h>

int main(int argc, char *argv[]){
    DIR *dir;
    struct dirent *file;
    struct dirent *file1;
    unsigned short min = SHRT_MAX;
    struct stat fileStat;

    dir = opendir(argv[1]); //Apro la cartella data in input

    //Se dir = NULL allora la cartella non esiste
    if (dir==NULL){
        printf("La directory non esiste");
    }
    
    //Leggo i file presenti nella cartella fintanto che non trovo il file più piccolo
    file1 = readdir(dir);
    while((file = readdir(dir))!=NULL){
        if (file1->d_reclen <= file->d_reclen){
            file1 = file;
            
        }
    }
    char resolved_path[PATH_MAX];
    realpath(argv[1], resolved_path); //Trovo il path della cartella
    char buff[1000];

    //char partialPath = strcat(resolved_path, "/");
    //char absolutePath = strcat(partialPath, file1->d_name);
   
    //printf("%s\n", absolutePath); //Unire il resolved path a /file->d_name
    sprintf(buff, "%s/%s", resolved_path, file1->d_name); //Trovo il path assoluto del file, unendo la stringa che contiene il path della cartella con il nome del file.
    printf("%s\n", buff);
    printf("%s\n",file1->d_name);
    
    //Stampo i permessi relativi al file appena individuato grazie all'uso della struct fileStat
    if (stat(buff, &fileStat)<0)
                return 1;
            printf((fileStat.st_mode & S_IROTH) ? "r" : "-");
            printf((fileStat.st_mode & S_IWOTH) ? "w" : "-");
            printf((fileStat.st_mode & S_IXOTH) ? "x" : "-");
            printf("\n");
    
}