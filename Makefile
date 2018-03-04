OBJS1 = restaurant.o
SOURCE1 = restaurant.c
OUT1 = restaurant
OBJS2 = customer.o
SOURCE2 = customer.c
OUT2 = customer
OBJS3 = doorman.o
SOURCE3 = doorman.c
OUT3 = doorman
OBJS4 = waiter.o
SOURCE4 = waiter.c
OUT4 = waiter

CC = gcc
FLAGS = -g -c
LIBS = -lpthread

all: restaurant customer doorman waiter 

restaurant: $(OBJS1)
	$(CC) -g $(OBJS1) -o $(OUT1) $(LIBS)
customer: $(OBJS2)
	$(CC) -g $(OBJS2) -o $(OUT2) $(LIBS)
doorman: $(OBJS3) $(OBJS_LIST)
	$(CC) -g $(OBJS3) $(OBJS_LIST) -o $(OUT3) $(LIBS)
waiter: $(OBJS4)
	$(CC) -g $(OBJS4) -o $(OUT4) $(LIBS)
restaurant.o: restaurant.c
	$(CC) $(FLAGS) $(SOURCE1)
customer.o: customer.c
	$(CC) $(FLAGS) $(SOURCE2)
doorman.o: doorman.c
	$(CC) $(FLAGS) $(SOURCE3)
waiter.o: waiter.c
	$(CC) $(FLAGS) $(SOURCE4)

clean:
	rm -f $(OBJS1) $(OBJS2) $(OBJS3) $(OBJS4) $(OUT1) $(OUT2) $(OUT3) $(OUT4)
