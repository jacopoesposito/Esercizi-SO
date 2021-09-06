/*Scrivere un programma C e Posix in ambiente Linux che preso in input il nome di una directory, ne
esamini il contenuto e individui tutti i file di dimensione maggiore di 10 Kb. Ad ogni file individuato si
assegnino i diritti di lettura scrittura ed esecuzione per gli altri (“others”) eccetto se il file individuato è
un link simbolico nel qual caso il nome del file è usato per creare una directory dello stesso nome in cui
creare un file conta.txt che contiene il numero di file precedentemente individuati.*/


#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <limits.h>
#include <string.h>

DIR *dir;
struct dirent *file;
struct stat fileStat;
struct stat dirStat;

char filePath[PATH_MAX];
char newDirPath[PATH_MAX];
int numFile = 0;

int main(int argc, char **argv){
	
	if(argc < 2){
		fprintf(stderr, "Errore input!\n");
	}


	if((dir = opendir(argv[1]))==NULL){
		perror("Impossibile aprire la directory");
	}
	
	while((file = readdir(dir))!=NULL){
		sprintf(filePath, "%s/%s", argv[1], file->d_name);
		if((stat(filePath, &fileStat))<0){
			perror("Errore Stat");
		}
		if(fileStat.st_size>10){
			numFile++;
			if(file->d_type == DT_LNK){
				sprintf(newDirPath, "%s/%sDir", argv[1], file->d_name);
				printf("%s\n", newDirPath);	
				if((stat(newDirPath, &dirStat))==-1){//Se la stat ritorna -1 allora la cartella non esiste e posso crearla
					mkdir(newDirPath, S_IRWXU);//Creo una nuova cartella dandogli il nome del file simbolico trovato e i permessi di lettura e scrittura per l'user
					chdir(newDirPath); //Apro la nuova cartella
					creat("conta.txt", S_IRWXU | S_IRWXG | S_IROTH); //Creo il file conta.txt
					int fd;
       					if((fd = open("conta.txt", O_RDWR))==-1){ //Apro il file descriptor in lettura e scrittura
            					fprintf(stderr, "ERRORE, APERTURA FILE\n");
       					}
					printf("%d\n", numFile);
					char tempbuff[13];
					sprintf(tempbuff, "%d", numFile);//Per riuscire a scrivere un intero nel file txt, converto l'intero in una rappresentazione testuale grazie a sprintf
					write(fd, tempbuff, strlen(tempbuff));//Scrivo il numero delle occorenze nel file
					close(fd);//Chiudo il file
					chdir(argv[1]);//Ritorno alla cartella principale
					char buff[PATH_MAX];
					getcwd(buff, sizeof(buff));
					printf("%s", buff);	
				}
			}else{
				chmod(filePath, S_IRWXU |  S_IROTH | S_IWOTH | S_IXOTH); //Attraverso chmod setto i permessi richiesti per i file
			}
		}
	
	}

}
