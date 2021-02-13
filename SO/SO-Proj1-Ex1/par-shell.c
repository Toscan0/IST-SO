/*	SO - 1 Entrega 
	
	Grupo: 25
	Tiago Henriques- 81633
	Pedro Cunha- 82343
	Tomas Zaky- 79690 */ 


#include <stdio.h>
#include <string.h>
#include <stdlib.h>						
#include <unistd.h>		
#include <sys/types.h>	
#include <sys/wait.h>   
#include <signal.h>		
#include "commandlinereader.h" 

#define TRUE 1
#define MAXARG 7

void prompt(); 


int main() {
	int pid, numTokens, estado;
	int contador = 0, i = 0;
	char *lstProcess[MAXARG]; /* Lista com todos os processos */

	prompt();
	
	while (TRUE){
		numTokens = readLineArguments(lstProcess, MAXARG);
		
		if(numTokens < 0){
			printf("Erro: acao impossivel\n");
			continue;
		}
		
		else if(numTokens == 0){
			continue;
		}
		
		else{
			if (strcmp(lstProcess[0], "exit") == 0)
				break;

			pid = fork();
			
			if (pid == -1) 
				perror("Erro: fork impossivel de executar\n");
			
			else if (pid == 0){ /* processo filho */
				execv(lstProcess[0], lstProcess);
				printf("Erro: ocorreu um erro durante o execv\n");
				exit(EXIT_FAILURE);
			}
			
			else{ /* processo pai */
				contador++;
				continue;
			}
		}
	}

	int *lstEstados = malloc(contador * sizeof(int)); /* Lista com o estado de cada processo */
	pid_t *lstPid = malloc(contador * sizeof(pid_t)); /* Lista com o pid de cada processo */
	
	/* Guarda o pid e o estado na lista correspondente */
	for(i = 0; i < contador; i++){ 
		lstPid[i] = wait(&estado);
		lstEstados[i] = estado;
	}

	/* Imprime o estado e o pid de cada processo*/
	for(i = 0; i < contador; i++){ 
		if(WIFEXITED(lstEstados[i]) == 1)
			printf("pid: %d   estado: %d\n", lstPid[i], WEXITSTATUS(lstEstados[i]));
	}
	
	/* Liberta a memoria alocada para as listas */
	free (lstEstados);
	lstEstados = NULL;
	free (lstPid);
	lstPid = NULL;
	
	return 0;
}


void prompt(){ 
	/* Introduz uma frase de boas vindas no terminal */
	printf("Introduza uma instrucao\n");
}