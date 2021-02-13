/*	Projeto de SO 
	
	Grupo: 25
	Tiago Henriques- 81633
	Pedro Cunha- 82343
	Tomas Zaky- 79690 */ 

#ifndef TERMINAL_H
#define TERMINAL_H

#include <stdlib.h>
#include <stdio.h>
	
typedef struct lstTerm_iitem {
	int pid; /* Pid do terminal em execução */
	struct lstTerm_iitem *next;
} lstTerm_iitem_t;

/* list_t */
typedef struct{
   lstTerm_iitem_t *first;
} listTerm_t;

/* Cria uma lista para colocar os pids dos terminais em execucao - reserva a memoria*/
listTerm_t* lstTerminal_new();
/* Destroi a lista com todos os terminais- liberta a memoria */
void lstTerminal_destroy(listTerm_t *list);
/* Insere um terminal na lista */
void inster_new_terminal(listTerm_t *list, int pid);
/* Remove um terminal da lista */
void remove_pid_terminal(listTerm_t *list, int pid);

#endif 