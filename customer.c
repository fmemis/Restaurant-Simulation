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
#include <time.h>


//this program imitates a customer of a restaurant.It is called by the restaurant program,not the user.
int main(int argc,char *argv[]) {
	//printf("I am the customer with pid %d\n",getpid());
	int i = 0,NumOfCustomers = 0,staytime = 0,shmid = 0;
	srand(time(NULL));

	//read arguments.No error check is needed.Restaurant program always calls with the right arguments
	while (i < argc) {
	    if (!strcmp(argv[i],"-s")) {
	    	shmid = atoi(argv[i + 1]);
	    }
	    else if (!strcmp(argv[i],"-n"))
	      	NumOfCustomers = atoi(argv[i + 1]);
	    else if (!strcmp(argv[i],"-d"))
	    	staytime = atoi(argv[i + 1]);
	    ++i;
 	}
 	int mytable;
 	void *shared = (SharedSeg*)shmat(shmid,(void*)0,0);
 	SharedSeg *s = (SharedSeg*)shared;
 	if ((int)shared == -1) perror("Attachment failed in customer");
 	sem_wait(&(s->mutex));
 	s->WaitingCustomers++;
 	sem_post(&(s->mutex));

  	//wait until the doorman calls.
 	sem_wait(&(s->DoorQueue));
 	s->SeatsNeeded = NumOfCustomers;
 	
 	//wake up the doorman.You gave him how many seats you need.
 	sem_post(&(s->DoorBlock));

 	//wait until the doorman shows you your table or kicks you out of the restaurant!!!
 	sem_wait(&(s->CustomerBlock));
 	table *tables;
 	if (s->allow == 1) {
 		//printf("I am the customer with pid %d,I wanted %d seats and I entered the restaurant\n",getpid(),NumOfCustomers);
 		mytable = s->table;
 		//printf("My table is %d\n",mytable);
 		
 		tables = shared + sizeof(SharedSeg) + mytable * sizeof(table);
 		tables->typeofcall = 1;
 		tables->groupid = getpid();
 		//tables->groupsize=NumOfCustomers;

 		//call for a waiter to order(typeofcall = 1)
 		sem_post(&(s->WaiterQueue));
 		//wait until he/she comes and take your order
 		sem_wait(&(tables->TableBlock));

 		
 		//printf("I have pid %d and my waiter is %d\n",getpid(),tables->waiterid);

 		//stay at the restaurant until you eat(a random time between 1 and 20 which has benn passed as an argument from restaurant)
 		sleep(staytime);
 		tables->typeofcall = 2;
 		//call the waiter for the check.
 		sem_post(&(s->WaiterQueue));

 		//wait until he comes.Pay and leave
 		sem_wait(&(tables->TableBlock));
 		sem_wait(&(s->mutex));
 		s->totalmoney += tables->bill;
 		sem_post(&(s->mutex));
 		tables->tablesbill += tables->bill;
 		//printf("I am %d paying %d euros\n",getpid(),tables->bill);
 		sleep(1);
 	}
 	else if (s->table == -1) {
 		printf("problem....\n");
 	}
 	else {
 		sem_wait(&(s->mutex));
 		s->groupsnotallowed++;
 		sem_post(&(s->mutex));
 		if ((s->groupsleft + s->groupsnotallowed) == s->totalcustomers)
 			s->close = 1;
 		if (shmdt(shared) == -1)
 			perror("Couldn't detach\n");
 		return 0;
 	}
 	sem_wait(&(s->mutex));
 	s->groupsleft++;
 	sem_post(&(s->mutex));
 	tables->groupsize = 0;
 	//printf("I am leaving and I have pid %d\n",getpid());
 	if ((s->groupsleft + s->groupsnotallowed) == s->totalcustomers)
 		s->close = 1;
 	if (shmdt(shared) == -1)
 		perror("Couldn't detach\n");
 	return 0;
 } 
