/* Realizzare un programma in C e Posix che realizza il seguente comportamento: un processo padre apre
un file ricevuto come argomento da riga di comando e crea due figli P1 e P2. P1 stampa il numero delle
lettere 'a' nel testo mentre P2 stampa il numero delle lettere 'b' nel testo. Il padre alla ricezione del
segnale SIGINT stampa la somma delle lettere contate dai processi figli e termina */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>

pid_t pid1, pid2;
struct stat fileStat;
int fd;
int countA = 0;
int countB = 0;

static void handler(int signo);

int main(int argc, char **argv){

    char *a = argv[1];
    if((fd = open(a, O_RDONLY))==-1){
        fprintf(stderr, "Errore file!");
    }
    if((fstat(fd, &fileStat))<0){
        fprintf(stderr, "Errore stat");
    }

    signal(SIGINT, handler);
    
    char buffer[fileStat.st_size];
    read(fd, buffer, sizeof(buffer));
    printf("Anakin Skywalker %ld\n", getpid());

    if((pid1=fork())==0){
        printf("Figlio 1 %ld\n", getpid());
        int count=0;
        for(int i = 0; i < fileStat.st_size; i++){
            if((buffer[i]=='a') || (buffer[i]=='A')){
                count++;
            }
        }
        printf("Count A->%d\n", count);
        return count;
    }
    if(pid1!=0){
        if((pid2=fork())==0){
            printf("Figlio 2 %ld\n", getpid());
            int count=0;
            for(int j = 0; j < fileStat.st_size; j++){
                if((buffer[j]=='b') || (buffer[j]=='B')){
                    count++;
                }
            }
            printf("Count B->%d\n", count);
            return count;
        }
    }
    if(pid1!=0 && pid2!=0){
        while(1){

        }
    }
}

static void handler(int signo){
    waitpid(pid1, &countA, 0); //Aspetto che il figlio finisca e carico il valore di ritorno nella variabile countA
    waitpid(pid2, &countB, 0); //Aspetto che il figlio finisca e carico il valore di ritorno nella variabile countB
    int return1 = WEXITSTATUS(countA); 
    int return2 = WEXITSTATUS(countB);
    printf("\nSomma lettere->%d\n", return1+return2);
    exit(0);
}