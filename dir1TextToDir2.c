/* Scrivere un programma C in ambiente Linux conforme allo standard Posix che accetta due directory, dir1
e dir2, da riga di comando cerca in dir1 il primo link simbolico che punta ad un file .txt, ne prende il
nome e ne crea una copia invertita all’interno della dir2 in un file con stesso nome ed estensione del file
individuato precedentemente. Alla copia bisogna assegnare i diritti di scrittura, lettura ed esecuzione per
il proprietario e sola lettura per il gruppo e per gli altri. */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <libgen.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

char *path1, *path2;
DIR *dir1;
DIR *dir2;
struct stat fileTxt;
struct dirent *file;
char *fileTrovato;
char symbolicFile[PATH_MAX];
char realPath[PATH_MAX];
char buffSearch[PATH_MAX];
int fd;

int main(int argc, char **argv){

    if(argc < 3){
        fprintf(stderr, "Inserisci i path delle due cartelle");
        exit(-1);
    }

    if((dir1=opendir(argv[1]))==NULL){
        fprintf(stderr, "Impossibile aprire la directory di origine non esiste!");
        exit(-2);
    }

    if((dir2=opendir(argv[2]))==NULL){
        fprintf(stderr, "Impossibile aprire la directory di destinazione");
        exit(-3);
    }

    while((file = readdir(dir1))!=NULL){
        if(file->d_type==DT_LNK){
            sprintf(symbolicFile, "%s/%s", argv[1], file->d_name); //Trovo il path al link simbolico concatenando il path della directory data in input col nome del file appena trovato
            realpath(symbolicFile, realPath); //Risolvo il link simbolico per trovare il path del file a cui punta
            sprintf(buffSearch, "%s/%s", realPath, "\0"); //Aggiungo una stringa di terminazione al path precedentemente trovato
            if(strstr(buffSearch, ".txt\0")){ //Verifico sia un file .txt
                fileTrovato = basename(realPath); //Recupero il nome del file dal path
                fd = open(fileTrovato, O_RDWR); //Apro il descrittore 
                break;
            }
        }
    }

    if(stat(realPath, &fileTxt)<0){ //Recupero la struct STAT del file appena recuperato
        fprintf(stderr, "Errore Stat");
        exit(-4);
    }

    
    char buffer[fileTxt.st_size]; //Creo un buffer dove inserire i dati del nuovo file 
    read(fd, buffer, sizeof(buffer)); //Leggo il file dato in input e scrivo il suo contenuto nel buffer
    closedir(dir1); //Chiudo la cartella di origine
    chdir(argv[2]); //Apro la cartella di destinazione
    int fd_nuovo; //Creo un nuovo FD per il nuovo file 
    creat(fileTrovato, S_IRWXU | S_IROTH | S_IRGRP); //Creo il file settando i permessi dati dalla traccia
    if((fd_nuovo = open(fileTrovato, O_RDWR))==-1){ //Apro il file appena creato
        fprintf(stderr, "Errore FD NUOVO");
    }
    write(fd_nuovo, buffer, sizeof(buffer)); //Scrivo i valori inseriti nel buffer nuovo_file all'interno del file appena creato
    closedir(dir2); //chiudo la cartella di destinazione
    close(fd_nuovo); //chiudo il file descriopto
    printf("OPERAZIONE TERMINATA\n");
}