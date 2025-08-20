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

/*
  shell prompt
 */

typedef struct
{
  int job_number;
  pid_t pid;
  char command[NL];
} Job;

Job jobs[NL];
int job_count = 0; /* Number of background jobs */

void prompt(void)
{
  // fprintf(stdout, "\n msh> ");
  fflush(stdout);
}

void handle_background(int sig)
{
  int status;
  pid_t pid;

  while ((pid = waitpid(-1, &status, WNOHANG)) > 0)
  {
    for (int i = 0; i < job_count; i++)
    {
      if (jobs[i].pid == pid)
      {
        printf("[%d]+ Done %s\n", jobs[i].job_number, jobs[i].command);
        // Remove the job from the list
        for (int j = i; j < job_count - 1; j++)
        {
          jobs[j] = jobs[j + 1];
        }
        job_count--;
        break;
      }
    }
  }
}

int main(int argk, char *argv[], char *envp[])
/* argk - number of arguments */
/* argv - argument vector from command line */
/* envp - environment pointer */

{
  int frkRtnVal;       /* value returned by fork sys call */
  int wpid;            /* value returned by wait */
  char *v[NV];         /* array of pointers to command line tokens */
  char *sep = " \t\n"; /* command line token separators    */
  int i;               /* parse index */

  int background;
  signal(SIGCHLD, handle_background);
  /* prompt for and process one command line at a time  */

  while (1)
  { /* do Forever */
    prompt();
    if (fgets(line, NL, stdin) == NULL)
    {
      fflush(stdin);
      if (feof(stdin))
      { /* non-zero on EOF  */

        /* fprintf(stderr, "EOF pid %d feof %d ferror %d\n", getpid(),
          feof(stdin), ferror(stdin)); */
        exit(0);
      }
      else
      {
        perror("EOF fgets NULL");
        continue;
      }
    }
    if (line[0] == '#' || line[0] == '\n' || line[0] == '\000')
      continue; /* to prompt */

    v[0] = strtok(line, sep);
    for (i = 1; i < NV; i++)
    {
      v[i] = strtok(NULL, sep);
      if (v[i] == NULL)
        break;
    }
    /* assert i is number of tokens + 1 */
    if (strcmp(v[i - 1], "&") == 0)
    {
      v[i - 1] = NULL;
      background = 1;
      i--;
    }
    else
    {
      background = 0;
    }
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
      perror("fork");
      break;
    }
    case 0: /* code executed only by child process */
    {
      execvp(v[0], v);
      perror("execvp");
      exit(EXIT_FAILURE);
    }
    default: /* code executed only by parent process */
    {
      if (background)
      {
        // If it's a background process, no need to wait for it explicitly
        // The SIGCHLD signal handler will take care of background processes
        jobs[job_count].job_number = job_count + 1;
        jobs[job_count].pid = frkRtnVal;
        jobs[job_count].command[0] = '\0';

        for (int j = 0; j < i; j++)
        {
          strcat(jobs[job_count].command, v[j]);
          if (j < i - 1)
          {
            strcat(jobs[job_count].command, " ");
          }
        }
        job_count++;

        // Print job start message
        printf("[%d] %d\n", jobs[job_count - 1].job_number, frkRtnVal);
        break;
      }
      else
      {
        wpid = wait(NULL);
        if (wpid == -1)
        {
          if (errno != ECHILD)
          {
            perror("wait");
          }
        }
      }
    }
    } /* switch */

    fflush(stdout);
  } /* while */
  return 0;
} /* main */