/* Scrivere un programma Trovafile.c che quando invocato con Trovafile PDF directory trovi 
tutti i file a partire dalla directory di nome directory che hanno come estensione PDF e ne verifichi i diritti di lettura per altri e la dimensione.
Individuato il file di dimensione massima, il programma deve creare un file con lo stesso nome e con estensione txt nella directory corrente 
in cui scrivere i nomi dei file. */


#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <limits.h>
#include <fcntl.h>
#include <string.h>

DIR *dir;
struct dirent *file;
struct stat fileStat;
char buffer[PATH_MAX];
char searchBuff[PATH_MAX];
char filePath[PATH_MAX];
char *nameFileMax;
char currentDir[PATH_MAX];
int value = INT_MIN;
int indice = 0;
int check = 0;

typedef struct{
    char *filename;
}nome; 

int main(int argc, char **argv){

    nome nlist[100]; //Array di struct nome, verrà utilizzato come list per inserire i nomi di tutti i file trovati

    if(argc < 2){
        fprintf(stderr, "Errore input");
    }

    if((dir = opendir(argv[1]))==NULL){
        perror("Impossibile aprire la cartella");
    }

    while((file = readdir(dir))!=NULL){
        sprintf(searchBuff, "%s%s", file->d_name, "\0"); //Aggiungo una stringa di terminazione al path precedentemente trovato
        
        if(strstr(searchBuff, ".pdf\0")){
            sprintf(filePath, "%s/%s", argv[1], file->d_name); //Mi ricavo il path assoluto del file concatenando il path dato in input al nome del file
            if((stat(filePath, &fileStat))<0){ //Verifico sia un file .pdf
                perror("Errore Stat\n");
            }
            if((fileStat.st_mode & S_IROTH)==S_IROTH){ //Verifico che il file abbia i permessi di lettura su altri
                nlist[indice].filename=file->d_name; //Aggiungo il file appena trovato alla "lista" di filename
                indice++;
            }
            if(fileStat.st_size>value){ //Controllo qual'è il file di dimensioni maggiori e mi salvo il nome.
                value = fileStat.st_size;
                nameFileMax = file->d_name;
                check = 1;
            }

        }
    }

    if(check == 1){
        getcwd(currentDir, sizeof(currentDir)); //Inserisco all'interno del buffer currentDir il path della cartella corrente
        chdir(currentDir); //Cambio cartella

        char buff[PATH_MAX];
        sprintf(buff, "%s%s", nameFileMax, ".txt"); //Concateno alla stringa nameFileMax l'estensione .txt
        creat(buff, S_IRWXU | S_IRGRP | S_IROTH); //Creo il nuovo file col nome desiderato e imposto i permessi 
        int fd;
        if((fd = open(buff, O_RDWR))==-1){
            perror("Errore apertura file");
        }

        for(int i = 0; i < indice; i++){
            printf("%s\n", nlist[i].filename);
        }
        int offset;
        int filesize; 

        for(int i = 0; i < indice; i++){
            filesize = strlen(nlist[i].filename); //Mi ricavo la lunghezza dell'iesima stringa da scrivere nel file
            offset += filesize+1; //Imposto l'offset in maniera tale da non riscontrare problemi di sovrascrittura
            write(fd, nlist[i].filename, filesize);
            write(fd, "\n", 1);
            lseek(fd, offset, SEEK_SET);
        }

        close(fd);
        closedir(dir);
    }else{
        printf("Nessun file PDF trovato\n");
        closedir(dir);
    }

    return 0;
    
}