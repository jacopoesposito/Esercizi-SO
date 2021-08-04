/*  Si realizzi un programma in C e Posix che dato in input il nome di un file, 
    lo cerchi all'interno della directory e se esso è regolare di dimensione minore ad 1 Kb 
    ne faccia il taglia e incolla nella directory del file puntato dal file simbolico presente nella directory.
*/


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


int main(int argc, char **argv){
    
    char buff[PATH_MAX];
    int fd;
    DIR *dir;
    DIR *newDir;
    struct stat statTrovato;
    struct dirent *file;
    struct dirent *fileTrovato;
    struct dirent *linkDirSup;
    char symbolicFile[1000];
    char resolved_path[PATH_MAX];
    char bufferStat[PATH_MAX];

    if(argc < 2){
        fprintf(stderr, "Errore input!\n");
        exit(-1);
    }
    
    if(getcwd(buff, sizeof(buff))==NULL){
        perror("getcwd() error");
        exit(-2);
    }

    char *nameFile = argv[1];
    dir = opendir(buff);

    if(dir == NULL){
        fprintf(stderr, "Directory inesistente!\n");
        exit(-3);
    }

    while((file = readdir(dir))!=NULL){
        if(file->d_type == DT_LNK){
            sprintf(symbolicFile, "%s/%s", buff, file->d_name); //Concateno nel buffer symbolicFile il path dato in input più il nome del file link simbolico trovato
            printf("\nPath associato al link simbolico e destinazione del file->%s\n", realpath(symbolicFile, resolved_path)); //in modo tale da trovarmi il path assoluto, dopodichè trovo tramite realpath il path a cui il file simbolico punta 
        }
        if(strcmp(file->d_name, nameFile)==0){
            sprintf(bufferStat, "%s/%s", buff, file->d_name); //Concateno nel bufferStat il path dato input più il nome del file dato in input
            if(stat(bufferStat, &statTrovato)<0){ //Accedo alla struct stat del file trovato
                exit(-3);
            }            
            if(statTrovato.st_size < 1024){ //Verifico che lo size del file sia inferiore a 1024byte
                fileTrovato = file;
                fprintf(stdout, "DESCRIZIONE FILE:\n Nome file->%s\n Size->%d\n Type->%u\n", fileTrovato->d_name, statTrovato.st_size, fileTrovato->d_type);
                if(fileTrovato->d_type == DT_REG){
                    fd = open(nameFile, O_RDWR); //Apro il descrittore del file
                    newDir = opendir(resolved_path); //Apro la cartella di destinazione, ottenuta precedentemente risolvendo il link simbolico
                    if(newDir != NULL){ //Verifico che effettivamente la cartella esista
                        char nuovo_file[statTrovato.st_size]; //Creo un buffer dove inserire i dati del nuovo file 
                        read(fd, nuovo_file, sizeof(nuovo_file)); //Leggo il file dato in input e scrivo il suo contenuto nel buffer
                        remove(argv[1]); //Rimuovo il file
                        closedir(dir);  //Chiudo la cartella di origine
                        chdir(resolved_path); //Apro la cartella di destinazione
                        int fd_nuovo; //Creo un nuovo FD per il nuovo file 
                        creat(argv[1], 00700); //Creo il file 
                        fd_nuovo = open(argv[1], O_RDWR); //Apro il file appena creato
                        write(fd_nuovo, nuovo_file, sizeof(nuovo_file)); //Scrivo i valori inseriti nel buffer nuovo_file all'interno del file appena creato
                        closedir(newDir); //chiudo la cartella di destinazione
                        close(fd_nuovo); //chiudo il file descrioptor
                        printf("OPERAZIONE TERMINATA\n");
                    }                
                }
            }
        }
    }

    return 0;
    
}