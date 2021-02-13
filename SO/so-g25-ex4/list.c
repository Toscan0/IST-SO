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

time_t get_time(lst_iitem_t *item){
	return item->dift;
}

list_t* lst_new(long int totaltime, int iter){
   list_t *list;
   list = (list_t*)malloc(sizeof(list_t));
   list->first = NULL;
   list->totaltime = totaltime;
   list->iter = iter;
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


void insert_new_process(list_t *list, int pid, time_t starttime){
	lst_iitem_t *item;

	item = (lst_iitem_t *) malloc (sizeof(lst_iitem_t));
	item->pid = pid;
	item->starttime = starttime;
	item->endtime = 0;
	item->dift = 0;
	item->status = 0; /* So depois de executar o processo e que sabemos se ocorreu erro */
	item->next = list->first;
	list->first = item;
}


void update_terminated_process(list_t *list, int pid, int status, time_t endtime, FILE *fp){ 
    lst_iitem_t *item;
    
    item = list->first;
    while (item != NULL){
		if (item->pid == pid){
	    	item->endtime = endtime;
	    	item->status = status;
	    	item->dift = endtime - (item->starttime);
	    	break;
		}
		else{
			item = item->next;
    	}
    }
    list->totaltime += item->dift;
    fprintf(fp, "iteracao %d\n", list->iter);
    fprintf(fp, "pid: %d execution time: %ld s\n", pid, item->dift);
    fprintf(fp, "total execution time: %ld s\n", list->totaltime);
    fflush(fp);
    list->iter++;
}

void lst_print(list_t *list){
	lst_iitem_t *item;

	printf("\nList of processes:\n");
	item = list->first;
	while (item != NULL){
		if(WIFEXITED(item->status)){
	  		printf("pid: %d exited normally; Status: %d.", item->pid, WEXITSTATUS(item->status));
		}
		else{
	  		printf("Pid: %d terminated without calling exit.", item->pid);
	  	}
		printf(" execution time: %0.f s\n", difftime(item->endtime, item->starttime));

		item = item->next;
	}
	printf("**End of list**\n");
}