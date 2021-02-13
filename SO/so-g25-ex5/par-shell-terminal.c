/*	Projeto de SO 
	
	Grupo: 25
	Tiago Henriques- 81633
	Pedro Cunha- 82343
	Tomas Zaky- 79690 */

/* Bibliotecas */
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>



#define TRUE 1
#define MAXARG 7 /* Comand + 5 arg + NULL */
#define BUFFERSIZE 100 /* readLineArguments */


int main(int argc, char **argv){
	int filePipe, pid, fileStats;
	char nameStats[50];
	char *buffer = (char*)malloc((sizeof(char))*BUFFER_SIZE);

	/* Pid do terminal */
	pid = getpid();
	printf("Pid do terminal: %d\n", pid);
	/* Abre o pipe com o nome que foi dado como argumento na linha de comandos do par-shell-terminal */
	if((filePipe = open(argv[1], O_WRONLY)) < 0){
		perror("open");
		exit(EXIT_FAILURE);
	}
	if(snprintf(buffer, BUFFERSIZE, "addPipe: %d\n", pid) < 0){
		perror("snprintf");
		exit(EXIT_FAILURE);
	}
	if(write(filePipe, buffer, BUFFERSIZE) == -1){
		perror("write");
		exit(EXIT_FAILURE);
	}
	while(TRUE){

		/* Le da linha de comandos */
		if(fgets(buffer, BUFFERSIZE, stdin) == NULL){
			perror("fgets");
			exit(EXIT_FAILURE);
		}
		/* O utilizador escreveu o comando exit */
		if(strcmp(buffer, "exit\n") == 0){
			if(snprintf(buffer, BUFFERSIZE, "exit %d\n", pid) < 0){
				perror("snprintf");
				exit(EXIT_FAILURE);
			}
			if(write(filePipe, buffer, BUFFERSIZE) == -1){
				perror("write");
				exit(EXIT_FAILURE);
			}
			break;
		}
		/* O utilizador escreveu o comando stats */
		else if(strcmp(buffer, "stats\n") == 0){
			if(snprintf(buffer, BUFFERSIZE, "stats %d\n", pid) < 0){
				perror("snprintf");
				exit(EXIT_FAILURE);
			}
			/* Cria o nome do pipe, par-shell-stats-PID, onde pid e o pid do terminal*/
			if(sprintf(nameStats, "/tmp/par-shell-stats-%d", pid) < 0){
				perror("sprintf");
				exit(EXIT_FAILURE);
			}
			/* Elimina o pipe se já houver um */
			if(unlink(nameStats) != -1){
				perror("apagado o pipe");
				exit(EXIT_FAILURE);
			}
			/* Cria um novo pipe */
			if(mkfifo(nameStats, S_IRUSR | S_IWUSR) < 0){
				perror("mkfifo");
				exit(EXIT_FAILURE);
			}
			/* Envia o comando stats para o par-shell*/
			if(write(filePipe, buffer, BUFFERSIZE) == -1){
  				perror("write");
  				exit(EXIT_FAILURE);
  			}
  			/* Abre o pipe */
  			if((fileStats = open(nameStats, O_RDONLY)) < 0){
  				perror("open");
  				exit(EXIT_FAILURE);
  			}
  			/* Le do pipe e coloca no buffer */
			if(read(fileStats, buffer, BUFFERSIZE) == -1){
				perror("read");
				exit(EXIT_FAILURE);
			}
			printf("%s\n", buffer);
			/* fecha o pipe */
			close(fileStats);
			/*Elimina o pipe -> so é gerado para passar a informação do stats*/
			if(unlink(nameStats) == -1){
				perror("unlink");
				exit(EXIT_FAILURE);
			}
		}
  		else{
  			if(write(filePipe, buffer, BUFFERSIZE) == -1){
  				perror("write");
  				exit(EXIT_FAILURE);
  			}
		}
	}
	close(filePipe);
	free(buffer);
	return 0;
}