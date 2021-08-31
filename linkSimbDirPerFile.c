/* Scorrere tutta la directory data in input e trovare un link simbolico che punti ad un'altra directory, 
dopodiché scorrere questa seconda directory e trovare al suo interno tutti i file regolari con permessi di scrittura per il gruppo */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/stat.h>
#include <limits.h>

DIR *dir;
DIR *directoryTrovata;
char buffer[PATH_MAX];
char linkSimbolico[PATH_MAX];
char pathTrovato[PATH_MAX];
struct dirent *file;
struct dirent *fileTrovato;
struct stat fileStat;

int main(int argc, char **argv){

    if(argc < 2){
        fprintf(stderr, "Errore input");
    }

    if((dir = opendir(argv[1]))==NULL){
        perror("Impossibile aprire la directory!");
    }

    while((file = readdir(dir))!=NULL){
        if(file->d_type==DT_LNK){
            sprintf(linkSimbolico, "%s/%s", argv[1], file->d_name);
            realpath(linkSimbolico, buffer);
            printf("%s\n", buffer);
        }
        if((directoryTrovata = opendir(buffer))!=NULL){
            chdir(buffer);
            break;
        }
        else{
            perror("Impossibile aprire la directory puntata dal link simbolico");
            exit(-1);
        }
    }

    while((fileTrovato = readdir(directoryTrovata))!=NULL){
        if(fileTrovato->d_type == DT_REG){
            if((stat(fileTrovato->d_name, &fileStat))<0){
                perror("Errore stat");
            }
            if(fileStat.st_mode & S_IWGRP){
                printf("Nome file->%s\n", fileTrovato->d_name);
                printf("Lunghezza file->%d\n", fileStat.st_size);
                printf((fileStat.st_mode & S_IRGRP) ? "r" : "-");
                printf((fileStat.st_mode & S_IWGRP) ? "w" : "-");
                printf((fileStat.st_mode & S_IXGRP) ? "x" : "-");
                printf("\n");
            }
        }
    }

    exit(-1);
}