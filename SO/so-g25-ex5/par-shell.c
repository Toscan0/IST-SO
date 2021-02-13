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
#include <signal.h>
#include <pthread.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>	
#include <sys/wait.h>
#include <sys/stat.h> 

/* Outros ficheiros */
#include "commandlinereader.h"
#include "list.h" 
#include "terminal.h"

/* Constantes */
#define STDIN 0
#define STDOUT 1
#define TRUE 1
#define BUFFERSIZE 100 /* readLineArguments */
#define FILE_BUFFER 50 /*leFicheiro */
#define MAXARG 7 /* Comand + 5 arg + NULL */
#define MAXPAR 4 /* Numrto processos executaveis de cada vez*/
#define NAMEFILE "log.txt"
#define NAMEPIPE "/tmp/par-shell-in"

/* Variaveis globais */
int gf_exit = 0, gf_CTRLC = 0, g_numTerminais = 0, g_filePipe, g_numChildren = 0;
list_t *g_list; /* Lista que contem todos os processos ativos */
listTerm_t *g_listTerminais; /* Lista que contem todos os par-shell-terminais(pid deles) em execucao */
pthread_mutex_t g_lock;
pthread_cond_t g_limite, g_sleep;
FILE *g_fp;

/* Protipos */
/* Imprime uma frase de boas vindas na shell */
void prompt(); 
void *tarefa_monitora();
/* Le o ficheiro- atualiza o tempo total e as interações */
void leFicheiro(FILE *fp, long int *totaltime, int *iter);
/* Bloqueia o mutex e verifica se foi bem bloqueado */
void mutex_lock(pthread_mutex_t* mutex, FILE *fp); 
/* Desbloqueia o mutex e verifica se foi bem desbloqueado */ 
void mutex_unlock(pthread_mutex_t* mutex, FILE *fp);
/* Inicia as variaveis de condicao e verifica se foram bem iniciadas */
void iniciaVariaveisCondicao(pthread_cond_t* condition1, pthread_cond_t* condition2);
/* Inicia o mutex e verifica a sua iniciacao */
void iniciaMutex(pthread_mutex_t* mutex);
/* Faz o pthread_cond_wait e verifica se foi bem feito */
void c_wait(pthread_cond_t* condition, pthread_mutex_t* mutex, FILE *fp);
/*Faz o pthread_cond_signal e verifica se ocorreu sem erros */
void c_signal(pthread_cond_t* condition, FILE *fp);
/* Fecha o ficheiro e ve se foi bem fechado */
void fileClose(FILE *fp);
/* CTRL+C e exit-global*/
void terminal_kill();
/* Faz o close e a verificacao */
void pipeClose(int pipe);
/* trata o CTRL+C*/
void trataSignal();
/* Mata os terminais */
void killAll();

int main(int argc, char** argv){
	int pid, pidTerm, numTokens, iter, fileProcess, fileStats;
	long int timetotal;	
	char fileName[50], nameStats[50], str1[100], str2[50]; 
	char *lstProcess[MAXARG]; /* Lista com todos os processos */
	char *buffer = (char*)malloc((sizeof(char))*BUFFERSIZE);
	pthread_t tarefa;

	signal(SIGINT, trataSignal);
	/* Criar variaveis de condicao  e verifica a sua criacao */
	iniciaVariaveisCondicao(&g_limite, &g_sleep);
	/* inicia mutex e verifica a sua inicializacao */
	iniciaMutex(&g_lock);
	/* Criar thread e verifica a sua criacao*/
	if(pthread_create(&tarefa, NULL, &tarefa_monitora, NULL) != 0){
		perror("pthread_create");
		exit(EXIT_FAILURE);
	}
	/* Abre o ficheiro e verifica se foi aberto sem erros */
	if((g_fp = fopen(NAMEFILE, "a+")) == NULL){
		perror("Ficheiro");
		exit(EXIT_FAILURE);
	}

	prompt();
	leFicheiro(g_fp, &timetotal, &iter);
	g_list = lst_new(timetotal, iter); 
	g_listTerminais = lstTerminal_new();

	/* E possivel ser necessario acrescentar /tmp/ ao nome do pipe para este funcionar nos pc's da RNL*/
	/* Se ja houver um pipe criado com este nome apaga-o */
	if(unlink(NAMEPIPE) != -1){
		printf("Foi apagado o pipe: /tmp/par-shell-in\n");
	}
	/*Cria o pipe "par-shell in", com as permissoes de acesso 0777 */
	if(mkfifo(NAMEPIPE, S_IRUSR | S_IWUSR) < 0){
		perror("mkfifo");
		exit(EXIT_FAILURE);
	}
	/* abre o pipe com nome */
	printf("A espera de um par-shell-terminal\n");
	if((g_filePipe = open(NAMEPIPE, O_RDONLY)) < 0){
		perror("open");
		exit(EXIT_FAILURE);
	}
	printf("Pronto para receber as suas intruções\n");
	pipeClose(STDIN);
	dup(g_filePipe);
	pipeClose(g_filePipe);
	while(TRUE){
		if(gf_CTRLC == 1){
			break;
		}
		numTokens = readLineArguments(lstProcess, MAXARG, buffer, BUFFERSIZE);

		if(numTokens < 0){
			printf("Erro: ao ler do pipe\n");
			continue;
		}
		else if(numTokens == 0){
			continue;
		}
		/* Foi executado um novo par-shell-terminal */
		else if(strcmp(lstProcess[0], "addPipe:") == 0){
			mutex_lock(&g_lock, g_fp);
			pidTerm = atoi(lstProcess[1]);
			inster_new_terminal(g_listTerminais, pidTerm);
			g_numTerminais++;
			printf("O terminal %d entrou em execucao\n", pidTerm);
	 		printf("Terminais em execucao: %d\n", g_numTerminais);
			mutex_unlock(&g_lock, g_fp);
			}
		/* O utilizador escreveu o comando exit(no par-shell-terminal) */
		else if(strcmp(lstProcess[0], "exit") == 0){
			mutex_lock(&g_lock, g_fp);
			pidTerm = atoi(lstProcess[1]);
			remove_pid_terminal(g_listTerminais, pidTerm);
			g_numTerminais--;
			printf("O terminal %d fez exit\n", pidTerm);
			printf("Terminais em execucao %d\n", g_numTerminais);			
			mutex_unlock(&g_lock, g_fp);
		}
		/* O utilizador escreveu o comando stats(no par-shell-terminal) */
		else if(strcmp(lstProcess[0], "stats") == 0){
			mutex_lock(&g_lock, g_fp);
			/* Recebe o pid do terminal */
			pidTerm = atoi(lstProcess[1]);
			/* Cria o nome do ficheiro */
			if(sprintf(nameStats, "/tmp/par-shell-stats-%d", pidTerm) < 0){
				perror("sprintf");
				fileClose(g_fp);
				exit(EXIT_FAILURE);
			}
			/* Abre o pipe */
  			if((fileStats = open(nameStats, O_WRONLY)) < 0){
  				perror("open");
  				exit(EXIT_FAILURE);
  			}
  			if(sprintf(str1, "Tempo total de execucao: %ld s   ", timetotal) < 0){
				perror("sprintf: str1");
				fileClose(g_fp);
				pipeClose(fileStats);
				exit(EXIT_FAILURE);
			}
			if(sprintf(str2, "Processos em execucao: %d", g_numChildren) < 0){
				perror("sprintf: str2");
				fileClose(g_fp);
				pipeClose(fileStats);
				exit(EXIT_FAILURE);
			}
			if(strcat(str1, str2) == NULL){
				perror("strcat(str1, str2)");
				fileClose(g_fp);
				pipeClose(fileStats);
				exit(EXIT_FAILURE);
			}
  			if(write(fileStats, str1, BUFFERSIZE) == -1){
  				perror("write");
  				fileClose(g_fp);
  				pipeClose(fileStats);
  				exit(EXIT_FAILURE);
  			}
  			mutex_unlock(&g_lock, g_fp); 
		}
		else if(strcmp(lstProcess[0], "exit-global") == 0){
			trataSignal();
			mutex_lock(&g_lock, g_fp);
			gf_CTRLC = 1;
			c_signal(&g_sleep, g_fp);
			mutex_lock(&g_lock, g_fp);
			break;
		}
		else{
			mutex_lock(&g_lock, g_fp);
			while(g_numChildren == MAXPAR){
				c_wait(&g_limite, &g_lock, g_fp);
			}
			mutex_unlock(&g_lock, g_fp);

			pid = fork();
			if (pid == -1){
				perror("fork");
				fileClose(g_fp);
				exit(EXIT_FAILURE);
			}
			else if (pid == 0){ /* processo filho */
				pid = getpid();
				/* Converte o inteiro pid numa string e junta ao nome do ficheiro */
				if(sprintf(fileName, "par-shell-out-%d.txt", pid) < 0){
					perror("sprintf");
					fileClose(g_fp);
					exit(EXIT_FAILURE);
				}
				/* Abre o ficheiro */
				if((fileProcess = open(fileName, O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR)) < 0){
					perror("open");
					fileClose(g_fp);
					exit(EXIT_FAILURE);
				}
				/* Fecha o ficheiro stdout */
				if(close(STDOUT) == -1){
					perror("close");
					fileClose(g_fp);
					exit(EXIT_FAILURE);
				}
				if(dup(fileProcess) == -1){
					perror("dup");
					fileClose(g_fp);
					exit(EXIT_FAILURE);
				}
				pipeClose(fileProcess);
				if(execv(lstProcess[0], lstProcess) == -1){
					perror("execv");
					fileClose(g_fp);
					exit(EXIT_FAILURE);
				}
			}
			else{ /* processo pai */
				mutex_lock(&g_lock, g_fp);
				g_numChildren++;
				insert_new_process(g_list, pid , time(NULL));
				c_signal(&g_sleep, g_fp);
				mutex_unlock(&g_lock, g_fp);
				continue;
			}
		}
		if(g_numTerminais == 0){
			if(unlink(NAMEPIPE) == -1){
				perror("unlink");
				exit(EXIT_FAILURE);
			}
			if(mkfifo(NAMEPIPE, S_IRUSR | S_IWUSR) < 0){
				perror("mkfifo");
				exit(EXIT_FAILURE);
			}
			printf("A espera de um par-shell-terminal\n");
			if((g_filePipe = open(NAMEPIPE, O_RDONLY)) < 0){
				perror("open");
				exit(EXIT_FAILURE);
			}
			printf("Pronto para receber as suas intruções\n");
		}
	}
	if(pthread_join(tarefa, NULL) != 0){
		perror("pthread_join");
		fileClose(g_fp);
		exit(EXIT_FAILURE);
	}
	lst_print(g_list);
	lst_destroy(g_list);
	lstTerminal_destroy(g_listTerminais);
	free(buffer);
	fileClose(g_fp);
	if(unlink(NAMEPIPE) == -1){
		perror("unlink");
		fileClose(g_fp);
		exit(EXIT_FAILURE);
	}
	if(pthread_mutex_destroy(&g_lock) != 0){
		perror("pthread_mutex_destroy");
		fileClose(g_fp);
		exit(EXIT_FAILURE);
	}
	if((pthread_cond_destroy(&g_sleep) != 0) || (pthread_cond_destroy(&g_limite) != 0)){
		perror("pthread_cond_destroy");
		fileClose(g_fp);
		exit(EXIT_FAILURE);
	}
	return 0;
}

void prompt(){
	printf("---Let the games begin---\n");
	printf("Limite de processos a ocorrer simultaneamente (MAXPAR): %d\n", MAXPAR);
}

void *tarefa_monitora(){
	int pid, estado;

	while(TRUE){
		if ((gf_exit == 1) && (g_numChildren == 0)){
			break;
		}

		mutex_lock(&g_lock, g_fp);
		while((g_numChildren == 0) && (gf_exit == 0)){
			c_wait(&g_sleep, &g_lock, g_fp);
		}
		mutex_unlock(&g_lock, g_fp);

		if (g_numChildren < 0){
			printf("Erro: numero de processos filho invalido");
			fileClose(g_fp);
			exit(EXIT_FAILURE);
		}
		else{
			pid = wait(&estado);
			mutex_lock(&g_lock, g_fp);
			update_terminated_process(g_list, pid, WEXITSTATUS(estado), time(NULL), g_fp);
			g_numChildren--;
			c_signal(&g_limite, g_fp);	
			mutex_unlock(&g_lock, g_fp);
		}
	}
	return NULL;
}

void leFicheiro(FILE *fp, long int *timetotal, int *iter){
	char file_buffer[FILE_BUFFER], temp[FILE_BUFFER];
	int it = 0;

	/*Le quantas linhas tem o ficheiro */
	while(feof(fp)) {
		if(fgets(file_buffer, FILE_BUFFER, fp) != NULL){
		   it++;
		}
		/* Verfica se o fgets ocorreu sem erros */
		if(!ferror(fp)){
		    perror("fgets");
		    fprintf(fp, "fgets failed in file %s at line %d\n", __FILE__,__LINE__-5);
		    exit(EXIT_FAILURE);
		}
	}
	/* Retira o tempo total de execucao do ficheiro */
	if(sscanf(file_buffer, "%s%s%s%ld%s", temp, temp, temp, timetotal, temp) == 0){
	  perror("sscanf");
	  fclose(fp);
	  exit(EXIT_FAILURE);
	}
	*iter = it/3;
	if(*iter == 0) *timetotal = 0;
}

void mutex_lock(pthread_mutex_t* mutex, FILE *fp){
	if(pthread_mutex_lock(mutex) != 0){
		perror("pthread_mutex_lock");
		fileClose(fp);
		exit(EXIT_FAILURE);
	}
}

void mutex_unlock(pthread_mutex_t* mutex, FILE *fp){
	if(pthread_mutex_unlock(mutex) != 0){	
		perror("pthread_mutex_unlock");
		fileClose(fp);
		exit(EXIT_FAILURE);
	}
}

void iniciaVariaveisCondicao(pthread_cond_t* condition1, pthread_cond_t* condition2){
	if((pthread_cond_init(condition1, NULL) != 0) || (pthread_cond_init(condition2, NULL) != 0)){
		perror("pthread_cond_init");
		/* nao e necessario fechar o ficheiro, porque ainda nao foi aberto */
	  	exit(EXIT_FAILURE);
	}
}

void iniciaMutex(pthread_mutex_t* mutex){
	if(pthread_mutex_init(mutex, NULL) != 0){
		printf("pthread_mutex_init");
		/* nao e necessario fechar o ficheiro, porque ainda nao foi aberto */
		exit(EXIT_FAILURE);
	}
}

void c_wait(pthread_cond_t* condition, pthread_mutex_t* mutex, FILE *fp){
  if (pthread_cond_wait(condition, mutex)){
    perror("pthread_cond_wait");
    fileClose(fp);
    exit(EXIT_FAILURE);
 }
}

void c_signal(pthread_cond_t* condition, FILE *fp){
  if (pthread_cond_signal(condition)){
    perror("pthread_cond_signal");
    fileClose(fp);
    exit(EXIT_FAILURE);
  }
}

void fileClose(FILE *fp){
	if(fclose(fp) != 0){
		perror("fileClose");
		exit(EXIT_FAILURE);
	}
}

void pipeClose(int pipe){
	if(close(pipe) == -1){
		perror("close");
		exit(EXIT_FAILURE);
	}
}

void trataSignal(){
	gf_CTRLC = 1;
	printf("\n");
	killAll();
	pipeClose(g_filePipe);
	pipeClose(STDIN);
}
void killAll(){
	lstTerm_iitem_t *item;

	item = g_listTerminais->first;
	while(item != NULL){
		kill(item->pid, SIGINT);
		item = item->next;
	}
	if(unlink(NAMEPIPE) == -1){
		perror("unlink");
		exit(EXIT_FAILURE);
	}
	exit(EXIT_SUCCESS);
}