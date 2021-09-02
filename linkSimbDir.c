/* Si realizzi un programma in C e Posix sotto Linux che data una directory da riga di comando,
ne elenchi il contenuto e, individuati i link simbolici, stampi i nomi dei file puntati, 
la dimensione e se hanno il bit-user id impostato */

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <libgen.h>
#include <sys/types.h>
#include <sys/stat.h>


int main(int argc, char **argv){

    DIR *dir;
    struct dirent *file;
    struct dirent *fileTrovato;
    struct stat fileStat;
    struct stat fileStat3;
    char buff[1000];
    char symbolicFile[1000];
    char resolved_path[PATH_MAX];

    if(argc > 2){
        printf("Errore input!");
    }

    dir = opendir(argv[1]);

    if(dir == NULL){
        printf("La directory non esiste");
    }

    printf("File nella directory:\n");
    while((file = readdir(dir))!=NULL){ //Leggo i file nella directory finche non è finita
        printf("%s\n", file->d_name); // Stampo il nome del file i-esimo
        sprintf(buff, "%s/%s", argv[1], file->d_name); //Concateno nel buffer le stringhe del path dato in input + il file name, in modo tale da trovarmi il path del file
        if(lstat(buff, &fileStat)<0){ //accedo alla struct stat del file associato al path buff
            return 1; //Ritorno 1 in caso di errore
        }
        if(S_ISLNK(fileStat.st_mode)){ //se il file è un link simbolico allora proseguo
            sprintf(symbolicFile, "%s/%s", argv[1], file->d_name); //Concateno nel buffer symbolicFile il path dato in input più il nome del file link simbolico trovato 
            printf("%s\n", realpath(symbolicFile, resolved_path)); //in modo tale da trovarmi il path assoluto, dopodichè trovo tramite realpath il path a cui il file simbolico punta 
            //printf("%s\n", basename(resolved_path));
            stat(basename(resolved_path), &fileStat3); //accedo alla struct stat del file puntato dal link simbolico grazie al metodo basename che fornito in input un path ritorna il nome del file
        
            printf("trovato un link simbolico: %s, nome file associato: %s, dimensione: %d, permersso lettura: %s, permesso scrittura: %s, permesso exec: %s, bit user id: %d\n", 
            file->d_name, basename(resolved_path), fileStat3.st_size, (fileStat3.st_mode & S_IRUSR) ? "r" : "-", (fileStat3.st_mode & S_IWUSR) ? "w" : "-", 
            (fileStat3.st_mode & S_IXUSR) ? "x" : "-", fileStat3.st_uid); //Leggo tutti i dati a cui sono interessato
            }
    
    }
}