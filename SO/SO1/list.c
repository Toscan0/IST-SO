/*	Projeto de SO
	
	Grupo: 25
	Tiago Henriques- 81633
	Pedro Cunha- 82343
	Tomas Zaky- 79690 */ 

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "list.h"


int get_pid(lst_iitem_t *item){
	return item->pid;
}

list_t* lst_new(){
   list_t *list;

   list = (list_t*)malloc(sizeof(list_t));
   list->first = NULL;
   
   return list;
}


void lst_destroy(list_t *list){
	struct lst_iitem *item, *nextitem;

	item = list->first;
	while (item != NULL){
		nextitem = item->next;
		free(item);
		item = nextitem;
	}
	free(list);
}


void insert_new_process(list_t *list, int pid){
	lst_iitem_t *item;

	item = (lst_iitem_t *) malloc (sizeof(lst_iitem_t));
	item->pid = pid;
	item->status = 0; /* So depois de executar o processo e que sabemos se ocorreu erro */
	item->next = list->first;
	list->first = item;
}


void update_terminated_process(list_t *list, int pid, int status){ 
    lst_iitem_t *item;
    
    item = list->first;
    while (item != NULL){
		if (item->pid == pid){
	    	item->status = status;
	    	break;
		}
		else{
			item = item->next;
    	}
    }
}
