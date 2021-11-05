#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <stdlib.h>

/**
 * Executes the command "cat scores | grep Lakers".  In this quick-and-dirty
 * implementation the parent doesn't wait for the child to finish and
 * so the command prompt may reappear before the child terminates.
 *
 */

int main(int argc, char **argv)
{
  int pipefd[2];
  int pipefd2[2];
  int pid;
  int pid2;
  
  char *cat_args[] = {"cat", "scores", NULL};
  char *grep_args[] = {"grep", "Lakers", NULL};
  char *sort_args[] = {"sort", NULL, NULL};
  
  if(argc < 1){
    printf("Please enter the grep argument\n");
    exit(0);
  }
  
  grep_args[1] = argv[1];
  
  // make a pipe (fds go in pipefd[0] and pipefd[1])
  pipe(pipefd);
  pipe(pipefd2);

  pid = fork();
  
  if (pid == 0) //child process running grep
  {
    pid2 = fork(); //second child process running sort
    if (pid2 == 0){
      // child handles sorting
      dup2(pipefd2[0], 0);
      close(pipefd[0]);
      close(pipefd[1]);
      close(pipefd2[1]);
      execvp("sort", sort_args);
      exit(0);
    }
    else if(pid2 < 0){
      perror("error");
    }
    else{
      // child executes grep
      // child gets here and handles "grep Villanova"
      // replace standard input with input part of pipe
      dup2(pipefd[0], 0);
      dup2(pipefd2[1], 1);
      // close unused hald of pipe
      close(pipefd[1]);
      close(pipefd2[0]);
      // execute grep
      execvp("grep", grep_args);
      
      wait(NULL);
      wait(NULL);
      exit(0);  
    } 
  }
 
  else //parent process running cat
  {
    // parent gets here and handles "cat scores"
    // replace standard output with output part of pipe
    dup2(pipefd[1], 1);
    // close unused unput half of pipe
    close(pipefd[0]);
    close(pipefd2[0]);
    close(pipefd2[1]);
    // execute cat
    execvp("cat", cat_args);
    exit(0);
  }
  exit(0);
  
}