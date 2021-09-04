/* Si realizzi un programma C e Posix sotto Linux che preso un nome di file come parametro (che
chiamiamo per semplicità file_input) controllerà per prima cosa che il numero di argomenti passati sia
corretto. Il programma apre tale file file_input, crea un processo figlio (chiamato ProcFiglio) che
scriverà nel file file_input una stringa letta da tastiera. Il padre attende il figlio e visualizza a video il
contenuto del file file_input
Ad esempio, se il programma viene lanciato in questo modo:
$ ./a.out <file_input>
Introduci una stringa e premi [ENTER]
Produrrà il seguente output:
$ Il padre ha letto la stringa <valore_stringa>  */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <limits.h>


pid_t ProcFiglio;
struct stat fileStat;

int main(int argc, char **argv){

    if(argc < 2){
        fprintf(stderr, "Errore input!");
    }
    
    int fd;
    if((fd = open(argv[1], O_RDWR))==-1){
        perror("Impossibile aprire il file");
    }
    ProcFiglio = fork();
    
    if(ProcFiglio == 0){  //Se sono il figlio allora procedo
        char buffer[PATH_MAX];
        fflush(stdin);
        printf("Introduci una stringa e premi [ENTER] PID->%ld\n", getpid());
        scanf("%[^\n]", &buffer); //Imposto la scanf in maniera tale che legga tutti i caratteri fino a /n spazi inclusi
        fflush(stdin);
        //printf("%s", buffer);
        write(fd, &buffer, strlen(buffer)); //Scrivo all'interno del file il contenuto di buffer 
        close(fd);
        exit(0);
    }

    wait(NULL);
    if((fstat(fd, &fileStat))<0){
        perror("Errore fstat");
    }
    char buffRead[fileStat.st_size]; 
    lseek(fd, 0, SEEK_SET); //Imposto di nuovo l'offset all'inizio in tal modo da riuscire a leggere la stringa
    read(fd, buffRead, sizeof(buffRead)); //Leggo all'interno del file e inserisco il contenuto letto in buffRead
    printf("Il processo padre PID->%ld ha letto la stringa: %s\n", getpid(), buffRead);
    close(fd);

      
    return 0;
}