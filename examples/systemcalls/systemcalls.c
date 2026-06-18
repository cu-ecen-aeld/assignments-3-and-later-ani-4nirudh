#include "systemcalls.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

/**
 * @param cmd the command to execute with system()
 * @return true if the command in @param cmd was executed
 *   successfully using the system() call, false if an error occurred,
 *   either in invocation of the system() call, or if a non-zero return
 *   value was returned by the command issued in @param cmd.
*/
bool do_system(const char *cmd)
{

/*
 * TODO  add your code here
 *  Call the system() function with the command set in the cmd
 *   and return a boolean true if the system() call completed with success
 *   or false() if it returned a failure
*/

  int ret = system(cmd);

  /*
   ### Checking the category of failure and what caused it ###
   *** Status Checkers ***
   - WIFEXITED: Returns true if the child process terminated normally
   - WIFSIGNALED: Returns true if the child process was terminated by a signal
   
   *** Status Value Extractors ***
   - WEXITSTATUS: Actual exit code (0 for success, 1 for failure)
   - WTERMSIG: This returns the number of signal that caused child process to terminate
   */

  if (ret == -1) {
    perror("FAILURE: system() call");
    return false;
  } 

  if (WIFEXITED(ret)) { 
      int exit_code = WEXITSTATUS(ret);
      printf("Command exited with error code: %d\n", exit_code);
      return (exit_code == 0); // True only if exit_code is 0 (success).
  }
  
  if (WIFSIGNALED(ret)) {
      int signal_num = WTERMSIG(ret);
      printf("Command killed by signal: %d\n", signal_num);
      return false;
    }
  

  return false;
}

/**
* @param count -The numbers of variables passed to the function. The variables are command to execute.
*   followed by arguments to pass to the command
*   Since exec() does not perform path expansion, the command to execute needs
*   to be an absolute path.
* @param ... - A list of 1 or more arguments after the @param count argument.
*   The first is always the full path to the command to execute with execv()
*   The remaining arguments are a list of arguments to pass to the command in execv()
* @return true if the command @param ... with arguments @param arguments were executed successfully
*   using the execv() call, false if an error occurred, either in invocation of the
*   fork, waitpid, or execv() command, or if a non-zero return value was returned
*   by the command issued in @param arguments with the specified arguments.
*/

bool do_exec(int count, ...)
{
  va_list args;
  va_start(args, count);
  char * command[count+1];
  int i;
  for(i=0; i<count; i++)
  {
      command[i] = va_arg(args, char *);
  }
  command[count] = NULL;
  // this line is to avoid a compile warning before your implementation is complete
  // and may be removed
  command[count] = command[count];
  // printf("\n######## command[0] %s #######\n", command[0]);
  // printf("\n######## command[1] %s #######\n", command[1]);
  // printf("\n######## count -> %d #######\n", count);

  va_end(args);

  /*
  * TODO:
  *   Execute a system command by calling fork, execv(),
  *   and wait instead of system (see LSP page 161).
  *   Use the command[0] as the full path to the command to execute
  *   (first argument to execv), and use the remaining arguments
  *   as second argument to the execv() command.
  *
  */

  pid_t pid;

  // Clone the process
  pid = fork(); 

  if (pid == -1) {
    perror("FAILURE: fork() call");

    // Return false to the the parent process
    return false;
  } 

  // Inside the child process
  if (pid == 0) {

    // Executing the command inside the child process
    int ret = execv(command[0], command);

    printf("###### Status code: %d\n", ret);
    
    if (ret == -1) {
      perror("FAILURE: execv() for 'echo' command");

      // Terminate the child process
      exit(EXIT_FAILURE);
    }
  }

  // Waiting for child process with PID 0 to finish
  int status;
  if (waitpid(pid, &status, 0) == -1) {
    perror("FAILURE: waitpid()");
    return false;
  }

  if (WIFEXITED(status)) {
    int exit_code = WEXITSTATUS(status);
    printf("The process exited normally with exit code: %d", exit_code);
    return (exit_code == 0);
  }

  return false;
}

/**
* @param outputfile - The full path to the file to write with command output.
*   This file will be closed at completion of the function call.
* All other parameters, see do_exec above
*/
bool do_exec_redirect(const char *outputfile, int count, ...)
{
    va_list args;
    va_start(args, count);
    char * command[count+1];
    int i;
    for(i=0; i<count; i++)
    {
        command[i] = va_arg(args, char *);
    }
    command[count] = NULL;
    // this line is to avoid a compile warning before your implementation is complete
    // and may be removed
    command[count] = command[count];


/*
 * TODO
 *   Call execv, but first using https://stackoverflow.com/a/13784315/1446624 as a refernce,
 *   redirect standard out to a file specified by outputfile.
 *   The rest of the behaviour is same as do_exec()
 *
*/
  printf("%d\n", count);
  printf("##### command[0] %s #####\n", command[0]);
  printf("##### command[1] %s #####\n", command[1]);
  printf("##### command[2] %s #####\n", command[2]);

  pid_t pid;
  pid = fork();
  if (pid == -1) {
    perror("FAILURE: fork() in do_exec_redirect()");
    return false;
  }

  // Starting the child process
  if (pid == 0) {
    int fd = open("testfile.txt", O_WRONLY | O_CREAT, 0644);
    if (fd == -1) {
      perror("FAILURE: open()");
      exit(EXIT_FAILURE);
    }

    if (dup2(fd, 1) == -1) {
      perror("FAILURE: dup2()");
      close(fd);
      exit(EXIT_FAILURE);
    }

    // Executing the command inside the child process
    int ret = execv(command[0], command);

    printf("###### Status code: %d\n", ret);
    
    if (ret == -1) {
      perror("FAILURE: execv() for 'echo' command");

      // Terminate the child process
      exit(EXIT_FAILURE);
    }
  }

  // Waiting for child process with PID 0 to finish
  int status;
  if (waitpid(pid, &status, 0) == -1) {
    perror("FAILURE: waitpid()");
    return false;
  }

  if (WIFEXITED(status)) {
    int exit_code = WEXITSTATUS(status);
    printf("The process exited normally with exit code: %d", exit_code);
    return (exit_code == 0);
  }

  va_end(args);

  return false;
}
