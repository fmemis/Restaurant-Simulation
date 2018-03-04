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

//this program imitates a waiter of the restaurant.It is called by the restaurant program,not by the user
int main(int argc,char *argv[]) {
	int i = 0,moneyamount = 0,period = 0,shmid = 0;
	srand(time(NULL));

	//read arguments.No error check is needed.Restaurant program always calls with the right arguments
	while (i < argc) {
	    if (!strcmp(argv[i],"-s")) {
	    	shmid = atoi(argv[i + 1]);
	    }
	    else if (!strcmp(argv[i],"-m"))
	      	moneyamount = atoi(argv[i + 1]);
	    else if (!strcmp(argv[i],"-d"))
	    	period = atoi(argv[i + 1]);
	    ++i;
 	}
 	void *shared = (SharedSeg*)shmat(shmid,(void*)0,0);
 	SharedSeg *s = (SharedSeg*)shared;
 	if ((int)shared == -1) perror("Attachment failed in customer");
 	int jobdone=0;

 	while (1) {
 		//wait for a customer call
 		sem_wait(&(s->WaiterQueue));
 		if (s->close == 1)
 			break;
 		table *tables;

 		//first see if anyone has call for a check.He/she has priority.If you encounter a table which wants to pay but its not your reponisiblity.wake up another waiter and continue your work.
 		for (i = 0;i < s->NumOfTables;++i) {
 			tables = shared + sizeof(SharedSeg) + i * sizeof(table);
 			if (tables->typeofcall == 2) {
 				if (getpid() == tables->waiterid) {
 					srand(time(NULL));
 					int m = rand() % moneyamount;
 					tables->bill = m;
 					int p = rand() % period + 1;
 					tables->typeofcall = 0;

 					//you need some time to be paid.
 					sleep(p);
 					jobdone = 1;

 					//the client has payed.He can live.
 					sem_post(&(tables->TableBlock));
 					break;
 				}
 				else {
 					//Not your table.Wake up another waiter to come and take the check.
 					sem_post(&(s->WaiterQueue));
 				}
 			}
 		}

 		//check if someone has asked to order.If yes take the order.
 		for (i = 0;i < s->NumOfTables;++i) {
 			tables = shared + sizeof(SharedSeg) + i * sizeof(table);
 			sem_wait(&(tables->mutex));
 			if (tables->typeofcall  == 1) {
 				//only you can be paid from this table!!!
 				tables->waiterid = getpid();

 				//printf("I am %d waiter and i serve the %d table with pid %d\n",tables->waiterid,i,tables->groupid);
 				int p = rand() % period + 1;
 				tables->typeofcall = 0;
 				sem_post(&(tables->mutex));

 				//you need sometime to take the order.
 				sleep(p);
 				jobdone = 1;
 				sem_post(&(tables->TableBlock));
 				break;
 			}
 			sem_post(&(tables->mutex));
 		}
 	}
 	//printf("I am the waiter with pid %d and my shift ended\n",getpid());
 	if (shmdt(shared) == -1)
 		perror("Couldn't detach\n");
 	return 0;
 }


