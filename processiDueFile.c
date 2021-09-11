/* Scrivere un programma in C e POSIX sotto Linux, che dati in input i path di due file crea un processo figlio e copia 
 * i contenuti del File 1 all'interno del File 2 */


#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <limits.h>
#include <string.h>

int fd1, fd2;

int main(int argc, char **argv){

	if(argc < 3){
		fprintf(stderr, "Errore input!");	
	}

	if((fd1 = open(argv[1], O_RDWR))==-1){
		perror("Impossibile aprire il file");
		exit(-1);
	}
	
	if((fd2 = open(argv[2], O_RDWR))==-1){
		perror("Impossibile aprire il file 2");
		exit(-2);
	}

	pid_t procFiglio;
	int fdTmp;
	creat("file_tmp.txt", S_IRWXU | S_IRGRP | S_IROTH );

	
	if((fdTmp = open("file_tmp.txt", O_RDWR)) == -1){
		perror("Errore apertura file temporaneo");
		exit(-3);
	}

	if((procFiglio = fork()) == 0){
		
		char buffer[PATH_MAX];
		read(fd1, &buffer, sizeof(buffer));


		write(fdTmp, &buffer, strlen(buffer));
		close(fd1);
		exit(0);
	}
	else{
		wait(NULL);
		char bufferWrite[PATH_MAX];
		lseek(fdTmp, 0, SEEK_SET);	
		read(fdTmp, &bufferWrite, sizeof(bufferWrite));
		printf("Stringa da incollare->%s\n", bufferWrite);
		write(fd2, &bufferWrite, strlen(bufferWrite));
		printf("Taglia e incolla completato!\n");
		close(fd2);
		remove("file_tmp.txt");
		exit(0);
	}



}
