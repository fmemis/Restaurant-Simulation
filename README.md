# Restaurant-Simulation
A restaurant simulation in C using sychronization between processes(extensive use of semaphores)

# Project Description
In this project we have 4 main programs which(all together) simulate a restaurant.The user executes the restaurant
program with the right arguments.Then this program initializes the shared segment(a part of memory where all the programs
will have access) and with the use of exec calls the other programs(doorman waiter and customer) with the right arguments.

