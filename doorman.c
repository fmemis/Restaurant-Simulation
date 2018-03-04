#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <semaphore.h>
#include "structs.h"

//this program imitates the doorman of a restaurant.It is called by the restaurant program,not by the user
int main(int argc,char *argv[]) {
	int i = 0,maxtime = 0,shmid = 0,end = 0;

	//read arguments.No error check is needed.Restaurant program always calls with the right arguments
	while (i < argc) {
	    if (!strcmp(argv[i],"-s")) {
	    	shmid = atoi(argv[i + 1]);
	    }
	    else if (!strcmp(argv[i],"-d"))
	    	maxtime = atoi(argv[i + 1]);
	    ++i;
 	}

 	//attach shared segment and the end of the process address space.
 	void *shared = (SharedSeg*)shmat(shmid,(void*)0,0);
 	SharedSeg *s = (SharedSeg*)shared;
 	if ((int)shared == -1) perror("Attachment failed in customer");
 	while (1) {

 		//if there are waiting customers
 		if (s->WaitingCustomers) {
 			//allow a customer process to continue its work
 			sem_post(&(s->DoorQueue));
 			//wait until customer process gives you the info about how many seats it needs(via variable SeatsNeeded in the SharedSeg)
 			sem_wait(&(s->DoorBlock));
 			int flag = 0;
 			int i;

 			//search for an empty table with the required seat capacity.
 			table *tables;
 			for (i  =  0;i < s->NumOfTables;++i) {
 				tables = shared + sizeof(SharedSeg) + i * sizeof(table);
 				if (tables->capacity >=  s->SeatsNeeded && tables->groupsize == 0) {
 					tables->groupsize = s->SeatsNeeded;
 					s->allow = 1;
 					s->table = i;
 					flag = 1;
 					//printf("I will put the group with %d customers in table %d\n",tables->groupsize,s->table);
 					break;
 				}
 			}

 			//if there is no space,pity for the client and us!!!
 			if (!flag) { 
 				s->allow = 0;
 				printf("I am sorry,there is no space for you.You have to leave\n");
 			}
 			sem_wait(&(s->mutex));
 			s->WaitingCustomers--;
 			sem_post(&(s->mutex));
 			sem_post(&(s->CustomerBlock));
 			end = 1;
 			sleep(2);
 		}

 		else
 			if (end) break; //program ends when all clients have been served.
 	}
 	
 	if (shmdt(shared) == -1)
 		perror("Couldn't detach\n");
 	return 0;
 }