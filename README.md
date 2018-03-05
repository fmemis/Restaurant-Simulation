# Restaurant-Simulation
A restaurant simulation in C using sychronization between processes(extensive use of semaphores)

## Project Description
In this project we have 4 main programs which(all together) simulate a restaurant.

### restaurant
  The user executes the restaurant program with the right arguments.Then this program initializes the shared 
  segment(a part of memory where all the programs will have access) and with the use of exec calls the other 
  programs(doorman waiter and customer) with the right arguments.Doorman is called just one time but waiter and 
  customer multiple times(There are multiple group of customers and multiple waiters in a restaurant)
  
 ### doorman
  When there is one or more empty tables at the restaurant the doorman checks if there is space for the first group 
  of customers(customer program) in the queue.If there is it finds them the appropriate table(according to group size)
  and tells them to sit.If there is no table  which the necessary seat capacity it asks them to leave and proceeds with
  the next group.
  Doorman programs finishes when the client queue is empty.
  
 ### customer
  Multiple customer programs are called(the user decides how many in the command line),each one with a small delay
  for the previous to simulate that the customers do not arrive all at once in a restaurant.Each customer program 
  represents a group of customers who all sit together in the same table.
  If allowed to enter, the customer calls for a waiter.When the waiter comes and takes the order, the customer stays
  to eat for a random time(how much a customer will stay at the restaurant is decided randomly from the restaurant 
  program which calls each customer with the corresponding argument).After that the customer calls for the check.
  When the waiter comes, the customer pays and after a brief time leaves the restaurant(the process running the particular
  customer program finishes its execution).
  
 ### waiter
 
 Multiple waiter programs are called(how many is decided in the configuration file which is passed as an argument from the
 command line when the restaurant program is called.)
 The waiters wait for a customer to call them for an order or to pay the check.They serve the customer and go back to waiting.
 The waiters processes all terminate when there is no customer in the restaurant or the customer queue.
 
 ## Compile and execute instructions
 
 To compile just run make.
 
 To execute restaurant: ./restaurant -n customers -I configurationfile -d printtime
 
 (the program will print statistics each "printtime" seconds)
 
 Example: ./restaurant -n 50 -I configurationfile.txt -d 5
 
 ## Notes
 
 The project demonstrates the use of semaphores and shared segments in order to achieve interprocess communication and 
 sychronization.

