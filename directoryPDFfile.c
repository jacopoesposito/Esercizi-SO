/*Scrivere un programma C e Posix in ambiente Linux che, fornita una directory DIR come argomento,
ricerchi il file con estensione pdf la cui dimensione è maggiore di 50Kb, puntato da un link simbolico
presente in DIR. Individuato il file pdf, il programma crea un file testo con estensione .txt che contiene il
nome del file pdf. Il file creato deve avere i permessi di lettura, scrittura ed esecuzione per il proprietario
ed il gruppo e sola lettura per gli altri.*/


#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <libgen.h>

DIR *dir;
struct dirent *file;
struct stat fileStat;
char *fileTrovato;
char buffSearch[PATH_MAX];
char symbolicFile[PATH_MAX];
char realPath[PATH_MAX];
int check;

int main(int argc, char **argv){


    if(argc < 2){
        fprintf(stderr, "ERRORE INPUT\n");
    }

    if((dir=opendir(argv[1]))==NULL){
        fprintf(stderr, "Impossibile aprire la directory!\n");
    }

    while((file=readdir(dir))!=NULL){
        if(file->d_type==DT_LNK){
            sprintf(symbolicFile, "%s/%s", argv[1], file->d_name); //Trovo il path al link simbolico concatenando il path della directory data in input col nome del file appena trovato
            realpath(symbolicFile, realPath); //Risolvo il link simbolico per trovare il path del file a cui punta
            sprintf(buffSearch, "%s/%s", realPath, "\0"); //Aggiungo una stringa di terminazione al path precedentemente trovato
            if(strstr(buffSearch,".pdf\0")){ //Verifico sia un file .pdf
                if((stat(realPath, &fileStat))==-1){
                    fprintf(stderr, "ERRORE FSTAT!\n");
                }
                printf("Size pdf trovato->%ld\n", fileStat.st_size);
                if(fileStat.st_size>50000){ //Verifico pesi di più di 50kb
                    printf("Path file trovato->%s\n", realPath);
                    fileTrovato =  basename(realPath); //Recupero il nome del file dal path
                    printf("Nome pdf trovato->%s\n", fileTrovato);
                    check = 1;
                    break;
                }
            }
        }
    }

    if(check==1){
        creat("file.txt", S_IRWXU | S_IRWXG | S_IROTH); //Creo il nuovo file con i permessi richiesti dalla traccia
        int fd;
        if((fd = open("file.txt", O_RDWR))==-1){ //Apro il file descriptor in lettura e scrittura
            fprintf(stderr, "ERRORE, APERTURA FILE\n");
        }
        write(fd, fileTrovato, strlen(fileTrovato)); //Scrivo il nome del file precedentemente trovato all'interno del file text appena
        printf("OPERAZIONE DI SCRITTURA TERMINATA\n");
        closedir(dir); //Chiudo la cartella
        close(fd); //Chiudo il fd
    }else{
        closedir(dir);
    }

    return 0;
}