/*********************************************************************
   Program  : miniShell                   Version    : 1.3
 --------------------------------------------------------------------
   skeleton code for linix/unix/minix command line interpreter
 --------------------------------------------------------------------
   File			: minishell.c
   Compiler/System	: gcc/linux

********************************************************************/

#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>

#define NV 20  /* max number of command tokens */
#define NL 100 /* input buffer size */
char line[NL]; /* command input buffer */

// build a structure of each jobs information
struct Job
{
    char command[NL];
    int job_ID;
    pid_t p_id;
};

int job_count = 0;
// An array of all jobs currently waiting to run
struct Job waiting_jobs[NL];

void to_background(int signal)
{
    pid_t sig_pid;
    int stat; //status value of child stored bu waitpid

    //ask where child is right now without suspending the caller
    //once a child dies remove them from the queue
    while ((sig_pid = waitpid(-1, &stat, WNOHANG)) > 0)
    {
        for (int k = 0; k < job_count; k++)
        {
            if (waiting_jobs[k].p_id == sig_pid)
            {
                //I had the plus in the wrong spot for a hot min
                printf("[%d]+ Done %s\n", waiting_jobs[k].job_ID, waiting_jobs[k].command);
                // Remove the job from the list
                for (int j = k; j < job_count - 1; j++)
                {
                    waiting_jobs[j] = waiting_jobs[j + 1];
                }
                job_count--;
                break;
            }
        }
    }
}

/*
    shell prompt
 */

void prompt(void)
{
    fflush(stdout);
}

/* argk - number of arguments */
/* argv - argument vector from command line */
/* envp - environment pointer */
int main(int argk, char *argv[], char *envp[])
{
    int frkRtnVal;       /* value returned by fork sys call */
    int wpid;            /* value returned by wait */
    char *v[NV];         /* array of pointers to command line tokens */
    char *sep = " \t\n"; /* command line token separators    */
    int i;               /* parse index */
    int background = 0;  // flag if command sent to background
    

    /* prompt for and process one command line at a time  */

    while (1)
    { /* do Forever */
        if (fgets(line, NL, stdin) == NULL)
        {
            fflush(stdin);
            // This if() required for gradescope
            if (feof(stdin))
            { /* non-zero on EOF  */
                exit(0);
            }
            else
            {
                perror("EOF fgets NULL");
                continue;
            }
        }
            if (line[0] == '#' || line[0] == '\n' || line[0] == '\000')
            {
                continue; /* to prompt */
            }

            v[0] = strtok(line, sep);
            for (i = 1; i < NV; i++)
            {
                v[i] = strtok(NULL, sep);
                if (v[i] == NULL)
                {
                    break;
                }
            }

            /* assert i is number of tokens + 1 */
            // check for &
            int last_input = i - 1;
            if (strcmp(v[last_input], "&") == 0)
            {
                background = 1;
                // remove & so execvp doesn't cry
                v[last_input] = NULL;
                i--; // do down beacuse we set the last input to null

                //handle the child process that goes to the background
                signal(SIGCHLD, to_background); 
            }
            else
            {
                background = 0;
            }

            // handle cd by doing outside the child process
            if (strcmp(v[0], "cd") == 0)
            {
                if (chdir(v[1]) != 0)
                {
                    perror("chdir");
                }
                continue;
            }

            /* fork a child process to exec the command in v[0] */
            switch (frkRtnVal = fork())
            {
            case -1: /* fork returns error to parent process */
            {
                perror("forking");
                break;
            }
            case 0: /* code executed only by child process */
            {

                execvp(v[0], v);
                perror("execvp is sad");
                exit(EXIT_FAILURE);
            }
            default: /* code executed only by parent process */
            {
                // sending to the background
                if (background)
                {
                    //take the input and put it in  to the next job
                    waiting_jobs[job_count].command[0] = '\0';
                    for (int j = 0; j < i; j++)
                    {
                        strcat(waiting_jobs[job_count].command, v[j]);
                        //add in a spce after commands and pray auto grader doesn't cry
                        if (j < i - 1)
                        {
                            strcat(waiting_jobs[job_count].command, " ");
                        }
                    }
                    //jobs need a number and a return value from the fork
                    waiting_jobs[job_count].job_ID = job_count + 1;
                    waiting_jobs[job_count].p_id = frkRtnVal;
                    
                    printf("[%d] %d \n", waiting_jobs[job_count].job_ID, frkRtnVal);
                    job_count++;
                    //this break prevented a mental breakdown 
                    break;
                }
                else
                {
                    //dont send to background, just do the thing and check it doesn't go wrong
                    wpid = wait(NULL);
                    if (wpid == -1)
                    {
                        if (errno != ECHILD)
                        {
                            perror("wait");
                        }
                    }
                }

                // REMOVE PRINTF STATEMENT BEFORE SUBMISSION
                // OK I WILL

                break;
            }
            } /* switch */
            //clear it all out just in case
            fflush(stdout);
        } /* while */
        return 0;
    } /* main */
