//Write a C program called even.c that takes the input parameter n from the command line and prints the first “n” even numbers (zero is even). 
//We want the program to run slowly, so that it executes long enough to receive a signal.  
//To achieve this, you should place a sleep(5) after every print statement.  
//Compile and run to test it works ok.

//Most Unix systems will understand SIGHUP and SIGINT.  
//Every signal has a default action associated with it, 
//for example the default action for SIGINT is to terminate the process. 