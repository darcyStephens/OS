//Write a C program called even.c that takes the input parameter n from the command line and prints the first “n” even numbers (zero is even). 
//We want the program to run slowly, so that it executes long enough to receive a signal.  
//To achieve this, you should place a sleep(5) after every print statement.  
//Compile and run to test it works ok.

//Most Unix systems will understand SIGHUP and SIGINT.  
//Every signal has a default action associated with it, 
//for example the default action for SIGINT is to terminate the process. 

#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>  // for memset
#include <stdlib.h>  // for atoi

void signal_handler(int signal)
{
    //switch statement for when i have to extend to more signals
    switch (signal) 
    {
        case SIGHUP:
        printf("Ouch!\n");
        break;

        case SIGINT:
        printf("Yeah!\n");
        break;

    }
}



int main(int argc, char *argv[])
{
    int n = atoi(argv[1]);

    //change the signal action
    signal(SIGINT, &signal_handler);
    signal(SIGHUP, &signal_handler);

    //print even numbers
    for(int i = 0; i< n * 2; i = i+2)
    {
        printf("%d \n", i);
        fflush(stdout);
        sleep(5);
    }
    return 0;
   
}
