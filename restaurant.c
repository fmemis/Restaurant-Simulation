#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <time.h>
#include "structs.h"

//Initialization program(the only one,the user calls)

int main(int argc,char *argv[]) {
    
    if (argc != 7) {
        perror("Wrong number of arguments");
        exit(0);
    }
    
    int totaltime = 1000;
	int i = 0,NumOfCustomers = 0,NumOfWaiters = 5,printtime = 0;
	FILE *in;
	char configfile[100];
    
    int flagsread[3];
    for (i = 0;i < 3; ++i)
        flagsread[i] = 0;
    i = 0;
	
    while (i < argc) {
	    if (!strcmp(argv[i],"-I")) {
	    	if ((in = fopen(argv[i + 1],"r")) == NULL) {
                perror("Problem opening file");
                exit(0);
            }
            flagsread[0] = 1;
	    	//strcpy(configfile,argv[i+1]);
	    }
	    if (!strcmp(argv[i],"-n")) {
	      	NumOfCustomers = atoi(argv[i + 1]);
            flagsread[1] = 1;
        }
	    else if (!strcmp(argv[i],"-d")) {
	    	printtime = atoi(argv[i + 1]);
            flagsread[2] = 1;
        }
	    ++i;
 	}
    for (i = 0; i < 3;++i) {
        if (flagsread[i] == 0) {
            perror("You didn't call the function with the right arguments");
            exit(0);
        }
    }

 	i = 0;

    //tables of size 2,4,5,6 and 8
 	int table2,table3,table4,table5,table6,table8;
    
    //read from configuration file
 	while (i < 5) {
 		char text[50];
 		char variable[30];
 		int num = 0;
 		fgets(text,50,in);
 		sscanf(text,"%s %d",variable,&num);
 		
 		if (!strcmp("waiters",variable))
 			NumOfWaiters = num;
 		else if (!strcmp("table2",variable))
 			table2 = num;	
 		else if (!strcmp("table4",variable))
 			table4 = num;
 		else if (!strcmp("table6",variable))
 			table6 = num;
 		else if (!strcmp("table8",variable))
 			table8 = num;
 		++i;
 	}

 	int tablenum = table2 + table4 + table6 + table8;
    //total size of shared space that will be attached to address space of each process.
 	int size = sizeof(SharedSeg) + tablenum * sizeof(table);
 	int shmid;
 	printf("Table num is %d\n",tablenum);
 	if ((shmid = shmget(IPC_PRIVATE,size,0666))  == -1) {
 		perror("Cannot create shared segment");
 		exit(0);
 	}
 	char shmidstring[30];
 	sprintf(shmidstring,"%d",shmid);
 	/*SharedSeg*/
    void *shared = (SharedSeg*)shmat(shmid,(void*)0,0);
 	SharedSeg *s=(SharedSeg*)shared;
 	if ((int)s == -1) {
 		perror("Attachment failed");
 		exit(1);
 	}

    //initialization of  SharedSeg semaphores and variables
 	sem_init(&(s->mutex),1,1);
    sem_init(&(s->DoorBlock),1,0);
    sem_init(&(s->DoorQueue),1,0);
    sem_init(&(s->WaiterQueue),1,0);
    sem_init(&(s->CustomerBlock),1,0);
 	s->table = -1;
 	s->allow = -1;
    s->close = 0;
    s->groupsleft = 0;
    s->groupsnotallowed = 0;
    s->totalmoney = 0;
    s->totalcustomers = NumOfCustomers;
 	s->NumOfTables = tablenum;

    //initialize tables.Space for them has been allocated at the end of the shared segment,after the struct SharedSeg.
 	table *tables;
 	for (i  =  0;i < tablenum;++i) {
 		tables = shared + sizeof(SharedSeg) + i * sizeof(table);
 		if (i < table2)
 			tables->capacity = 2;
 		else if (i < table2 + table4)
 			tables->capacity = 4;
 		else if (i < table2 + table4 + table6)
 			tables->capacity = 6;
 		else if (i < table2 + table4 + table6 + table8)
 			tables->capacity = 8;
 		tables->waiterid = 0;
 		tables->groupsize = 0;
        tables->typeofcall = 0;
        tables->groupid = 0;
        tables->tablesbill = 0;
        sem_init(&(tables->TableBlock),1,0);
        sem_init(&(tables->mutex),1,1);
 	}
    pid_t pid;
    srand(time(NULL));
    for (i  =  0;i < NumOfWaiters;++i) {

        //fork  NumOfWaiters new processes and call the waiter program on them.
        if ((pid = fork()) == -1) {
            perror("fork failure");
            exit(2);
        }
        if (pid == 0) {
            execlp("./waiter","waiter","-s",shmidstring,"-m","100","-d","3",NULL);
        }

    }

    //fork a process for the doorman
     if ((pid = fork()) == -1) {
    	perror("fork failure");
    	exit(2);
    }
    if (pid == 0)
    	execlp("./doorman","doorman","-s",shmidstring,"-d","100",NULL);

    //fork NumofCustomers new processes and call the customer program on them(sleep a little between because not all customers come at once.)
    int numofseats;
    for (i = 0;i < NumOfCustomers;++i) {
        sleep(1);
    	if ((pid = fork()) == -1) {
			perror("fork failure");
			exit(2);
		}
		if (pid == 0) {
            srand(time(NULL));
            sleep(1);
			numofseats = rand() % 8 + 1;
			char numofseatsstring[30];
			sprintf(numofseatsstring,"%d",numofseats);
            int staytime = rand() % 20 + 1;
            char staytimestring[20];
            sprintf(staytimestring,"%d",staytime);
			execlp("./customer","customer","-s",shmidstring,"-n",numofseatsstring,"-d",staytimestring,NULL);
		}
    }

    //print some statistics about what's happening at the restaurant
    int printfraction = totaltime / printtime;
    for (i = 0;i < printfraction;++i) {
        sleep(printtime);
        printf("The total number of groups left is %d\n",s->groupsleft);
        printf("The total number of groups not allowed to enter is %d\n",s->groupsnotallowed);
        printf("The sum of total money collected is %d\n",s->totalmoney);
        int people = 0;
        int j = 0;
        for (j = 0;j < tablenum;++j) {
            tables = shared + sizeof(SharedSeg) + j * sizeof(table);
            if (tables->groupsize > 0) {
                printf("Table %d has a group of %d customers\n",j,tables->groupsize);
                printf("The table's %d total bills have a value of %d\n",j,tables->tablesbill);
                people += tables->groupsize;
            }
           // if (i == printfraction -1)
               //printf("The table's %d total bills have a value of %d\n",j,tables->tablesbill);
        }
        printf("The current total number of people in the restaurant is %d\n",people);
        //free all resources and close the restaurant.
        if (s->close) {
            for (i = 0;i < NumOfWaiters;++i)
                sem_post(&(s->WaiterQueue));
            sleep(1);
            if (shmdt(shared) == -1)
                perror("Couldn't detach\n");
            if (shmctl(shmid, IPC_RMID, 0) == -1)
                perror ("Could not remove SharedSeg.");
            break;
        }
        printf("\n\n");
    }
    printf("The restaurant is closing\n");
    return 0;
}




















