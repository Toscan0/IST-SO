/*	SO - 1 Entrega 
	
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

/* outros ficheiros*/
#include "commandlinereader.h"
#include "list.h" 

/* Constantes*/
#define TRUE 1
#define MAXARG 7 /* Comand + 5 arg + NULL */
#define BUFFERSIZE 100
#define MAXPAR 4


void prompt(); /* imprime uma frase de boas vindas na shell*/
void *tarefa_monitora();

/* variaveis globais */
int gf_exit = 0;
int g_numChildren = 0;
//crias uma variavel po mutex
pthread_mutex_t g_lock;
list_t *g_list;
sem_t s_limite;
sem_t s_sleep;

int main(int argc, char* argv[]){
	int time_tk = 0; /* time_tweak */
	int pid, numTokens;	
	pthread_t tarefa;
	char *lstProcess[MAXARG]; /* Lista com todos os processos */
	char *buffer = (char*)malloc((sizeof(char*))*BUFFERSIZE);
	
	/* Criar semaforos */
	if((sem_init(&s_limite, 0, MAXPAR) != 0 ) || (sem_init(&s_sleep, 0, 0))){
	  printf("Erro: ao criar o semaforo");
	  exit(EXIT_FAILURE);
	}
	
	/* Criar mutex */
	if(pthread_mutex_init(&g_lock, NULL)){
		printf("Erro: ao criar mutex\n");
		exit(EXIT_FAILURE);
	}

	/* Criar thread*/
	if(pthread_create(&tarefa, NULL, &tarefa_monitora, NULL) != 0){
		printf("Erro: ao criar tarefa\n");
		exit(EXIT_FAILURE);
	}

	prompt();
	g_list = lst_new(); /* inicia a lista */
	
	if(argc > 1){
	  time_tk = atoi(argv[1]);
	}
	
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
				pthread_mutex_lock(&g_lock);
				sem_post(&s_sleep);
				gf_exit = 1;
				pthread_mutex_unlock(&g_lock);
				break;
			}
			
			sem_wait(&s_limite);
			pid = fork();
			if (pid == -1){
				printf("Erro: fork impossivel de executar\n");
			}

			else if (pid == 0){ /* processo filho */
				execv(lstProcess[0], lstProcess);
				printf("Erro: ocorreu um erro durante o execv\n");
				exit(EXIT_FAILURE);
			}
			else{ /* processo pai */
				sem_post(&s_sleep);
				pthread_mutex_lock(&g_lock);
				g_numChildren++;
				insert_new_process(g_list, pid , time(NULL));
				pthread_mutex_unlock(&g_lock);
				continue;
			}
		}
	}
	
	pthread_join(tarefa, NULL);
	lst_print(g_list, time_tk);
	lst_destroy(g_list);
	free(buffer);
	pthread_mutex_destroy(&g_lock);
	sem_destroy(&s_limite);
	sem_destroy(&s_sleep);
	return 0;
}


/* Introduz uma frase de boas vindas no terminal */
void prompt(){ 
	printf("Introduza as suas instrucoes\n");
}

void *tarefa_monitora(){
	int pid, estado;

	while(TRUE){

		if (gf_exit == 1 && g_numChildren == 0){
			break;
		}
		if (g_numChildren < 0){
			printf("Erro: numero de processos filho invalido");
			exit(EXIT_FAILURE);
		}
		else if(g_numChildren == 0){
			sem_wait(&s_sleep);
		}
		else{
			pid = wait(&estado);
			sem_post(&s_limite);
			pthread_mutex_lock(&g_lock);
			update_terminated_process(g_list, pid, WEXITSTATUS(estado), time(NULL));
			g_numChildren--;
			pthread_mutex_unlock(&g_lock);
		}
	}
	return NULL;
}