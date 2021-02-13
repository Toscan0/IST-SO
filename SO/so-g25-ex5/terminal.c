/*	Projeto de SO 
	
	Grupo: 25
	Tiago Henriques- 81633
	Pedro Cunha- 82343
	Tomas Zaky- 79690 */ 

#include <stdlib.h>
#include <stdio.h>
#include "terminal.h"

listTerm_t* lstTerminal_new(){
   listTerm_t *list;

   list = (listTerm_t*) malloc(sizeof(listTerm_t));
   list->first = NULL;

   return list;
}

void lstTerminal_destroy(listTerm_t *list){
	struct lstTerm_iitem *item, *nextitem;

	item = list->first;
	while (item != NULL){
		nextitem = item->next;
		free(item);
		item = nextitem;
	}
	free(list);
}

void inster_new_terminal(listTerm_t *list, int pid){
	lstTerm_iitem_t *item;

	item = (lstTerm_iitem_t *) malloc(sizeof(lstTerm_iitem_t));
	item->pid = pid;
	item->next = list->first;
	list->first = item;
}

void remove_pid_terminal(listTerm_t *list, int pid){
	lstTerm_iitem_t *item, *anterior;

	item = list->first;
	while(item != NULL && anterior != NULL){
		if(pid == item->pid){
			lstTerm_iitem_t *temp = item;
			anterior->next = item->next;
			free(temp);
			return;
		}
		anterior = item;
		item = item->next;
	}	
}

