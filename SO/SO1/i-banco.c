/*
// Projeto SO - exercicio 1, version 1
// Sistemas Operativos, DEI/IST/ULisboa 2016-17
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>                     
#include <unistd.h>     
#include <sys/types.h>  
#include <sys/wait.h>   
#include <signal.h>
#include "commandlinereader.h"
#include "contas.h"

#define COMANDO_DEBITAR "debitar"
#define COMANDO_CREDITAR "creditar"
#define COMANDO_LER_SALDO "lerSaldo"
#define COMANDO_SIMULAR "simular"
#define COMANDO_SAIR "sair"
#define COMANDO_SAIR_AGORA "agora"
#define BUFFER_SIZE 100
#define MAXARGS 3
#define TRUE 1



int g_sair = 0, g_sair_agora;

int main (int argc, char** argv) {
    int process = 0, estado, i;
    char *args[MAXARGS + 1];
    char buffer[BUFFER_SIZE];

    inicializarContas();

    printf("Bem-vinda/o ao i-banco\n\n");
      
    while (TRUE) {
        int numargs;
    
        numargs = readLineArguments(args, MAXARGS+1, buffer, BUFFER_SIZE);

        /* EOF (end of file) do stdin ou comando "sair" */
        if (numargs < 0 || (numargs == 1 && (strcmp(args[0], COMANDO_SAIR) == 0))){
            printf("--\ni-banco vai terminar\n");        
            g_sair = 1;
            g_sair_agora = 0;
            break;
        }

        else if((strcmp(args[0], COMANDO_SAIR) == 0) && (strcmp(args[1], COMANDO_SAIR_AGORA) == 0)){
            printf("saindo agora\n");
            g_sair_agora = 1;
            g_sair = 0;
            break;
        }
    
        else if (numargs == 0){
            /* Nenhum argumento; ignora e volta a pedir */
            continue;
        }
            
        /* Debitar */
        else if (strcmp(args[0], COMANDO_DEBITAR) == 0) {
            int idConta, valor;
            if (numargs < 3) {
                printf("%s: Sintaxe inválida, tente de novo.\n", COMANDO_DEBITAR);
	           continue;
            }

            idConta = atoi(args[1]);
            valor = atoi(args[2]);

            if (debitar (idConta, valor) < 0){
	           printf("%s(%d, %d): OK\n\n", COMANDO_DEBITAR, idConta, valor);
            }
            else{
                printf("%s(%d, %d): OK\n\n", COMANDO_DEBITAR, idConta, valor);
            }
        }

        /* Creditar */
        else if (strcmp(args[0], COMANDO_CREDITAR) == 0) {
            int idConta, valor;
            if (numargs < 3) {
                printf("%s: Sintaxe inválida, tente de novo.\n", COMANDO_CREDITAR);
                continue;
            }

            idConta = atoi(args[1]);
            valor = atoi(args[2]);

            if (creditar (idConta, valor) < 0){
                printf("%s(%d, %d): Erro\n\n", COMANDO_CREDITAR, idConta, valor);
            }
            else{
                printf("%s(%d, %d): OK\n\n", COMANDO_CREDITAR, idConta, valor);
            }
        }

        /* Ler Saldo */
        else if (strcmp(args[0], COMANDO_LER_SALDO) == 0) {
            int idConta, saldo;

            if (numargs < 2) {
                printf("%s: Sintaxe inválida, tente de novo.\n", COMANDO_LER_SALDO);
                continue;
            }
            idConta = atoi(args[1]);
            saldo = lerSaldo (idConta);
            if (saldo < 0){
                printf("%s(%d): Erro.\n\n", COMANDO_LER_SALDO, idConta);
            }
            else{
                printf("%s(%d): O saldo da conta é %d.\n\n", COMANDO_LER_SALDO, idConta, saldo);
            }
        }

        /* Simular */
        else if (strcmp(args[0], COMANDO_SIMULAR) == 0) {

            pid_t pid = fork();

            if (pid < 0){
                perror("Erro no fork\n");
                exit(1);
            }
            /* Processo filho */
            else if (pid == 0){
                simular(atoi(args[1]));
                execv(args[0], args);
                printf("erro\n");
                exit(EXIT_FAILURE); 
            }
            /*Processo Pai*/
            else{
                process++;
                continue;
            }  
        }
        else {
            printf("Comando desconhecido. Tente de novo.\n");
        }
    }
    
    int *lstEstados = malloc(process * sizeof(int)); /* Lista com o estado de cada processo */
    pid_t *lstPid = malloc(process * sizeof(pid_t)); /* Lista com o pid de cada processo */
    
    /* Guarda o pid e o estado na lista correspondente */
    for(i = 0; i < process; i++){ 
        lstPid[i] = wait(&estado);
        lstEstados[i] = estado;
    }

    /* Imprime o estado e o pid de cada processo*/
    for(i = 0; i < process; i++){ 
        if(WIFEXITED(lstEstados[i]) == 1){
            if(g_sair == 1){
            printf("FILHO TERMINADO (PID=%d, terminou normalmente)\n", lstPid[i]);
            }
            else{
                printf("FILHO TERMINADO (PID=%d, terminou abrubtamente)\n", lstPid[i]);
            }
        }
    }
  printf("--\ni-banco terminou \n");

  return 0;
}
