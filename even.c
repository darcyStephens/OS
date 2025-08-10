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
        printf("Ouch! \n");
        break;

        case SIGINT:
        printf("Yeah! \n");
        break;

        //perform default action of all other signals
        //doing this to extend my code in case I need to handle lots of signals in the future
        //realised this is dumb, keeping it anyways
        default:
        {
            struct sigaction act;
            //set structure to zeros to prevent errors
            memset(&act, 0, sizeof(act));
            //handler is default
            act.sa_handler = SIG_DFL;
            sigaction(signal, &act, NULL);
        }
        //re-raise to run signal again
        raise(signal);
    }
}

void change_signal_action(void)
{
    struct sigaction act;
    memset(&act, 0, sizeof(act));

    act.sa_handler = &signal_handler;

    //put all possible signals through my handler to save me adding code here in the future
    //again something that is probably not needed. Whoever is marking this can you please tell me if this is pointless?
    for(int sig = 1; sig < NSIG; sig++)
    {
        if(sig == SIGKILL || sig == SIGSTOP) continue;
        sigaction(sig,&act, NULL);
    }
}

int main(int argc, char *argv[])
{
    int n = atoi(argv[1]);

    change_signal_action();

    //print even numbers
    for(int i = 0; i<= n; i = i+2)
    {
        printf("%d \n", i);
        sleep(2);
    }
    return 0;
   
}
