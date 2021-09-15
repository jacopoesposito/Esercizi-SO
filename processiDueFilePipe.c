/* Scrivere un programma in C e POSIX sotto Linux, che dati in input i path di due file crea un processo figlio e copia 
 * i contenuti del File 1 all'interno del File 2 */



#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <limits.h>
#include <string.h>

int fd1, fd2;
int pfd[2];
pid_t pid1;

int main(int argc, char **argv){
	
	pipe(pfd);

	if(argc < 3){
		fprintf(stderr, "Errore input");
		exit(-1);
	}
	
	if((fd1 = open(argv[1], O_RDONLY))==-1){
		perror("Errore apertura file1:");
		exit(-2);
	}

	if((fd2 = open(argv[2], O_WRONLY))==-1){
		perror("Errore apertura file 2:");
		exit(-3);
	}
	

	if((pid1 = fork())==0){
		struct stat fileStat;
		if((stat(argv[1], &fileStat))==-1){
			perror("Errore fstat file 1");
			exit(-4);
		}
		char buffer[fileStat.st_size];
		read(fd1, &buffer, sizeof(buffer));
		printf("Stringa da copiare: %s", buffer);
		close(pfd[0]);
		write(pfd[1], &buffer, strlen(buffer));
		close(pfd[1]);
		exit(0);
	}else{
		waitpid(pid1, NULL, 0);
		struct stat fileStat;
		if((stat(argv[1], &fileStat))==-1){
			perror("Errore fstat file 1");
		}
		close(pfd[1]);
		char buffer[fileStat.st_size];
		read(pfd[0], &buffer, sizeof(buffer));
		printf("%s\n", buffer);
		close(pfd[0]);
		write(fd2, &buffer, strlen(buffer));
		return 0;
	}

}
