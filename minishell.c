/*********************************************************************
********************************************************************/

#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

#define NV 20			/* max number of command tokens */
#define NL 100			/* input buffer size */
char line[NL];	/* command input buffer */

struct Job
{
    pid_t pid;
    char command[NL];
    int job_ID;
};

struct Job running_jobs[NL];
int job_count = 0;

//function that will put a command into the background
void to_background(int signal)
{
  pid_t pid;
  int status;

  while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
    for (int itr = 0; itr < job_count; itr++) {
      if (running_jobs[itr].pid == pid) {
        printf("%s job with job number: %d has finished running\n", running_jobs[itr].command, running_jobs[itr].job_ID);
        for (int jit = itr; jit < job_count - 1; jit++) {
              running_jobs[jit] = running_jobs[jit + 1];
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
  //fprintf(stdout, "\n msh> ");
  fflush(stdout);
}


/* argk - number of arguments */
/* argv - argument vector from command line */
/* envp - environment pointer */
int main(int argk, char *argv[], char *envp[])
{
   int frkRtnVal;	    /* value returned by fork sys call */
   int wpid;		        /* value returned by wait */
  char *v[NV];	        /* array of pointers to command line tokens */
  char *sep = " \t\n";  /* command line token separators    */
  int i;		          /* parse index */
  int background; //background flag for when a command has & at the end
  signal(SIGCHLD, to_background);

    /* prompt for and process one command line at a time  */

  while (1) {			/* do Forever */
    prompt();
    fgets(line, NL, stdin);
    fflush(stdin);

    if (feof(stdin)) {		/* non-zero on EOF  */

      fprintf(stderr, "EOF pid %d feof %d ferror %d\n", getpid(),
	      feof(stdin), ferror(stdin));
      exit(0);
    }
    if (line[0] == '#' || line[0] == '\n' || line[0] == '\000'){
      continue;			/* to prompt */
    }

    v[0] = strtok(line, sep);
    for (i = 1; i < NV; i++) {
      v[i] = strtok(NULL, sep);
      if (v[i] == NULL){
	      break;
      }
    }
    /* assert i is number of tokens + 1 */

    //check if we are sending this input to a background process
    if (strcmp(v[i-1], "&") == 0) {
      background = 1;
      v[i-1] = NULL;  // remove the '&' token
      i--;
      printf("Sending ");
      for (int j = 0; j < i; j++) {
      printf("%s ", v[j]);
    }
    printf(" Job ID: %d to run in the background \n", running_jobs[i].job_ID+1);
}

    else
    {
        background = 0;
    }

    /* fork a child process to exec the command in v[0] */
    switch (frkRtnVal = fork()) {
      case -1:			/* fork returns error to parent process */
      {
	      break;
      }
      case 0:			/* code executed only by child process */
      {
        //printf("child process executing \n");
        execvp(v[0], v);
        perror("execvp failed");
        exit(EXIT_FAILURE);
      }
      default:			/* code executed only by parent process */
      {
        if(background)
        {
          //fill in all the commands for this job
          running_jobs[job_count].command[0] = '\0';
          for(int k = 0; v[k] != NULL; k++) {strcat(running_jobs[job_count].command, v[k]); strcat(running_jobs[job_count].command, " ");}
          //printf("Stored command: %s\n", running_jobs[job_count].command); //check we are storing commands

          running_jobs[job_count].pid = frkRtnVal;
          running_jobs[job_count].job_ID = job_count +1;
          //move to the next job
          job_count++;


        }
        else
        {wpid = wait(0); //returns PID of terminated child, returns -1 if no children exist
        //printf("%s done \n", v[0]);
        }
        break;
      }
    }	
     fflush(stdout);			/* switch */
  }				/* while */
  return 0;
}				/* main */
