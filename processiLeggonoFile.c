/*Realizzare un programma C e Posix sotto Linux che con l'uso dei segnali sincronizzi un processo padre ed
un processo figlio che scrivono n volte (con n passato da riga di comando), alternandosi, dal FILE1 contenente caratteri alfabetici e dal FILE2 contenente caratteri numerici rispettivamente,
nella prima e la seconda posizione di un file temporaneo creato opportunamente.
Ogni volta che il processo figlio ha scritto il proprio carattere invia un segnale ad un proprio processo figlio che provvede a leggere ed a stampare a video il contenuto del file temporaneo. */


#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <limits.h>

void signalHandler(int signo);

int fd;
int indice;
int indice1 = 0;
int indice2 = 0;

int main(int argc, char **argv){

	int n = atoi(argv[1]);
	
	pid_t pidFiglio;
	pid_t pidNipote;

	signal(SIGUSR1, signalHandler);
	signal(SIGUSR2, signalHandler);
	signal(SIGINT, signalHandler);

	creat("tmpFile.txt", S_IRWXU);


	if((fd = open("tmpFile.txt", O_RDWR))==-1){
		perror("Errore apertura file");
	}	
	//fprintf(stdout, "%ld", getpid());
	
	sleep(1);
	pid_t pidNonno = getpid();

	if((pidFiglio = fork())==0){
		pidNipote = fork();
		while(1){
			if(pidNipote == 0){ //Sono il nipote
				pause(); //Mi metto in pausa e aspetto il segnale dal padre
				kill(pidNonno, SIGINT); //Invio il segnale al nonno
			}
			if(pidNipote != 0){ //Sono il padre
				pause(); //Mi metto in pausa e aspetto il segnale dal nonno
				//sleep(1);
				kill(pidNipote, SIGUSR2); //Invio il segnale al figlio
			}
		}	
	}else{
		while(indice != n){ //Sono il nonno
			sleep(5);
			int fd1;
			char buffer[PATH_MAX];
			fd1 = open("file1.txt", O_RDONLY);
			lseek(fd1, indice, SEEK_SET);//Imposto il cursore a indice in modo da leggere sempre il carattere iesimo
			read(fd1, &buffer, 1);//Leggo l'iesimo carattere
			//close(fd1);
			lseek(fd, 0, SEEK_SET);//Imposto il cursore a 0 per scrivere nella prima posizione del file tmp.txt
			write(fd, &buffer, 1);//Scrivo nel file il carattere contenuto in buffer
			kill(pidFiglio, SIGUSR1);//Invio il segnale per sbloccare il figlio
			indice++;//Imposto l'indice per il ciclo e il cursore
			//sleep(5);
			//indice1++;
			pause(); //Mi metto in pausa e aspetto il segnale dal nipote
		}
	}

}

void signalHandler(int signo){
	
	if(signo == SIGUSR1){
		int fd2;
		int  number;
		fd2 = open("file2.txt", O_RDONLY); //apro il file2
		lseek(fd2, indice2, SEEK_SET); //Imposto il cursore a indice2 in modo da leggere sempre il numero i-esimo
		read(fd2, &number, 1); //Leggo l'iesimo numero
		//close(fd2);
		lseek(fd, 1, SEEK_SET); //Imposto l'offset a uno per scrivere nella posizione 1 del file temporaneo
		write(fd, &number, 1); //Scrivo il contenuto di number nel file temporaneo 
		lseek(fd, 0, SEEK_SET); //Imposto l'offset a 0 per il nonno
		indice2++;
	}
	if(signo == SIGUSR2){
		char buffer2[PATH_MAX];
		read(fd, &buffer2, sizeof(buffer2));
		//close(fd);
		write(1, &buffer2, strlen(buffer2));
	}
	if(signo == SIGINT){
		return ;
	}

}
