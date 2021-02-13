/*	Projeto de SO 
	
	Grupo: 25
	Tiago Henriques- 81633
	Pedro Cunha- 82343
	Tomas Zaky- 79690 */

/* Bibliotecas */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>						
#include <unistd.h>		
#include <sys/types.h>	
#include <sys/wait.h>   
#include <signal.h>
#include <pthread.h>
#include <time.h>
#include <semaphore.h>
#include <errno.h>

/* Outros ficheiros */
#include "commandlinereader.h"
#include "list.h" 

/* Constantes */
#define TRUE 1
#define MAXARG 7 /* Comand + 5 arg + NULL */
#define BUFFERSIZE 100
#define MAXPAR 4 /* Numrto processos executaveis de cada vez*/
#define NAMEFILE "log.txt"
#define FILE_BUFFER 50

/* Variaveis globais */
int gf_exit = 0, g_numChildren = 0;
list_t *g_list;
pthread_mutex_t g_lock;
pthread_cond_t g_limite, g_sleep;
FILE *g_fp;

/* Protipos */
/* Imprime uma frase de boas vindas na shell */
void prompt(); 
void *tarefa_monitora();
void leFicheiro(FILE *fp, long int *totaltime, int *iter);
/* Bloqueia o mutex e verifica se foi bem bloqueado */
void mutex_lock(); 
/* Desbloqueia o mutex e verifica se foi bem desbloqueado */ 
void mutex_unlock();
/* Inicia as variaveis de condicao e verifica se foram bem iniciadas */
void iniciaVariaveisCondicao();
/* Inicia o mutex e verifica a sua iniciacao */
void iniciaMutex();

int main(int argc, char** argv){
	int pid, numTokens, iter;
	long int timetotal;	
	pthread_t tarefa;
	char *lstProcess[MAXARG]; /* Lista com todos os processos */
	char *buffer = (char*)malloc((sizeof(char*))*BUFFERSIZE);
	
	/* Criar variaveis de condicao  e verifica a sua criacao */
	iniciaVariaveisCondicao();
	/* inicia mutex e verifica a sua inicializacao */
	iniciaMutex();
	/* Criar thread e verifica a sua criacao*/
	if(pthread_create(&tarefa, NULL, &tarefa_monitora, NULL) != 0){
		printf("Error");
		/* nao e necessario fechar o ficheiro, porque ainda nao foi aberto */
		exit(EXIT_FAILURE);
	}

	g_fp = fopen(NAMEFILE, "a+"); /* abre o ficheiro */
	/* Verifica se o ficheiro foi aberto sem erros */
	if(g_fp == NULL){
		perror("Error");
		exit(EXIT_FAILURE);
	}
	
	prompt();
	leFicheiro(g_fp, &timetotal, &iter);
	g_list = lst_new(timetotal, iter); /* inicia a lista */

	while (TRUE){
		numTokens = readLineArguments(lstProcess, MAXARG, buffer, BUFFERSIZE);
		
		if(numTokens < 0){
			printf("Erro: acao impossivel\n");
			continue;
		}
		else if(numTokens == 0){
			continue;
		}
		else{
			if (strcmp(lstProcess[0], "exit") == 0){
				mutex_lock();
				gf_exit = 1;
				if(pthread_cond_signal(&g_sleep) != 0){
					perror("Error");
					fclose(g_fp);
					exit(EXIT_FAILURE);
				}
				mutex_unlock();
				break;
			}
			
			mutex_lock();
			while(g_numChildren == MAXPAR){
				if(pthread_cond_wait(&g_limite, &g_lock) != 0){
					perror("Error");
					fclose(g_fp);
					exit(EXIT_FAILURE);
				}
			}
			mutex_unlock();

			pid = fork();
			if (pid == -1){
				printf("Erro: fork impossivel de executar\n");
			}
			else if (pid == 0){ /* processo filho */
				execv(lstProcess[0], lstProcess);
				printf("Erro: ocorreu um erro durante o execv\n");
				fclose(g_fp);
				exit(EXIT_FAILURE);
			}
			else{ /* processo pai */
				mutex_lock();
				g_numChildren++;
				insert_new_process(g_list, pid , time(NULL));
				if(pthread_cond_signal(&g_sleep) != 0){
					perror("Error");
					fclose(g_fp);
					exit(EXIT_FAILURE);
				}
				mutex_unlock();
				continue;
			}
		}
	}

	pthread_join(tarefa, NULL);
	lst_print(g_list);
	lst_destroy(g_list);
	free(buffer);
	fclose(g_fp);
	pthread_mutex_destroy(&g_lock);
	pthread_cond_destroy(&g_sleep);
	pthread_cond_destroy(&g_limite);
	return 0;
}

/* Introduz uma frase de boas vindas no terminal */
void prompt(){ 
	printf("Introduza as suas instrucoes:\n");
}

void *tarefa_monitora(){
	int pid, estado;

	while(TRUE){
		if (gf_exit == 1 && g_numChildren == 0){
			break;
		}
		if (g_numChildren < 0){
			printf("Erro: numero de processos filho invalido");
			fclose(g_fp);
			exit(EXIT_FAILURE);
		}
		else if(g_numChildren == 0){
			mutex_lock();
			if(pthread_cond_wait(&g_sleep, &g_lock) != 0){
				perror("Error");
				fclose(g_fp);
				exit(EXIT_FAILURE);
			}
			mutex_unlock();
		}
		else{
			pid = wait(&estado);
			mutex_lock();
			update_terminated_process(g_list, pid, WEXITSTATUS(estado), time(NULL), g_fp);
			g_numChildren--;	
			if(pthread_cond_signal(&g_limite) != 0){
				perror("Error");
				fclose(g_fp);
				exit(EXIT_FAILURE);
			}	
			mutex_unlock();
		}
	}
	return NULL;
}


void leFicheiro(FILE *fp, long int *timetotal, int *iter){
	char file_buffer[FILE_BUFFER], temp[FILE_BUFFER];
	int it = 0;
	while(!feof(g_fp)) {
		fgets(file_buffer, FILE_BUFFER, g_fp);
		it++;
	}
	sscanf(file_buffer, "%s%s%s%ld%s", temp, temp, temp, timetotal, temp);
	*iter = it/3;
	if(*iter == 0) *timetotal = 0;
}

void mutex_lock(){
	if(pthread_mutex_lock(&g_lock) != 0){
		perror("Error");
		fclose(g_fp);
		exit(EXIT_FAILURE);
	}
}

void mutex_unlock(){
	if(pthread_mutex_unlock(&g_lock) != 0){	
		perror("Error");
		fclose(g_fp);
		exit(EXIT_FAILURE);
	}
}

void iniciaVariaveisCondicao(){
	if((pthread_cond_init(&g_sleep, NULL) != 0) || (pthread_cond_init(&g_limite, NULL) != 0)){
		perror("Error");
		/* nao e necessario fechar o ficheiro, porque ainda nao foi aberto */
	  	exit(EXIT_FAILURE);
	}
}

void iniciaMutex(){
	if(pthread_mutex_init(&g_lock, NULL) != 0){
		printf("Error");
		/* nao e necessario fechar o ficheiro, porque ainda nao foi aberto */
		exit(EXIT_FAILURE);
	}
}