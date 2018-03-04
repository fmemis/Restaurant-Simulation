#include <semaphore.h>

typedef struct SharedSeg {
	sem_t mutex;
	sem_t DoorQueue;
	sem_t DoorBlock;
	sem_t CustomerBlock;
	sem_t WaiterQueue;
	int WaitingCustomers;
	int SeatsNeeded;
	int BarCurrSize;
	int table;
	int NumOfTables;
	int allow;
	int close;
	int groupsleft; //groups that have already left the restaurant
	int groupsnotallowed;
	int totalmoney;
	int totalcustomers;
}SharedSeg;

typedef struct table {
	int capacity;
	int waiterid;
	int groupsize;
	int typeofcall; //1 for order,2 for check
	int bill;
	int tablesbill; //total amount of money that has been accumulated from all customers of this table
	int groupid;
	sem_t TableBlock;
	sem_t mutex;	
}table;