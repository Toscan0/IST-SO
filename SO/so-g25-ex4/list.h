/*	Projeto de SO 
	
	Grupo: 25
	Tiago Henriques- 81633
	Pedro Cunha- 82343
	Tomas Zaky- 79690 */ 

#ifndef LIST_H
#define LIST_H

#include <stdlib.h>
#include <stdio.h>
#include <time.h>



/* lst_iitem - each element of the list points to the next element */
typedef struct lst_iitem {
   int pid;
   int status; /* acrescentamos o estado do processo */
   time_t starttime;
   time_t endtime;
   time_t dift;
   struct lst_iitem *next;
} lst_iitem_t;

/* list_t */
typedef struct {
   lst_iitem_t * first;
   int iter;
   long int totaltime;
} list_t;



/* lst_new - allocates memory for list_t and initializes it */
list_t* lst_new(long int totaltime, int iter);

/* lst_destroy - free memory of list_t and all its items */
void lst_destroy(list_t *);

/* insert_new_process - insert a new item with process id and its start time in list 'list' */
void insert_new_process(list_t *list, int pid, time_t starttime);

/* update_teminated_process - updates endtime of element with pid 'pid' */
void update_terminated_process(list_t *list, int pid, int status, time_t endtime, FILE *fp);

/* lst_print - print the content of list 'list' to standard output */
void lst_print(list_t *list);

/* devolve o pid do item */
int get_pid(lst_iitem_t *item);
 
/* devolve o tempo de execucao do item */
time_t get_time(lst_iitem_t *item);

#endif 