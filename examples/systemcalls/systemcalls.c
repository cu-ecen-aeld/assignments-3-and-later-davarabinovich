
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include "systemcalls.h"


#define BOURNE_SHELL_FULL_PATH "/bin/sh"


/**
 * @param cmd the command to execute with system()
 * @return true if the command in @param cmd was executed
 *   successfully using the system() call, false if an error occurred,
 *   either in invocation of the system() call, or if a non-zero return
 *   value was returned by the command issued in @param cmd.
*/
bool do_system(const char * cmd)
{
	int child_process_retval;
	child_process_retval = system(cmd);
	return (bool)child_process_retval;
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
	bool is_succeed = true;
	
    va_list args;
    va_start(args, count);
    char * command[count+1]; // +1 for NULL-termination
    for(int i = 0; i < count; i++)
        command[i] = va_arg(args, char *);
    command[count] = NULL;

	pid_t child_pid = fork();
	int exec_retval;
	if (child_pid == 0)
	{
		exec_retval = execv(command[0], &command[1]);
		if (exec_retval == -1)
			exit(1);
	}
	
	int child_status;
	pid_t waited_pid = waitpid(child_pid, &child_status, 0);
	if (waited_pid == -1 || waited_pid != child_pid || WEXITSTATUS(child_status) != 0)
		is_succeed = false;

    va_end(args);
    return is_succeed;
}

/**
* @param outputfile - The full path to the file to write with command output.
*   This file will be closed at completion of the function call.
* All other parameters, see do_exec above
*/
bool do_exec_redirect(const char * outputfile, int count, ...)
{
	bool is_succeed = true;
	
    va_list args;
    va_start(args, count);
    char * command[count+4];  // +4 for -c option of shell, for NULL-termination, and for redirection
	
	command[0] = "-c";
    for(int i = 1; i < count+1; i++)
        command[i] = va_arg(args, char *);
    command[count+1] = ">";
    command[count+2] = (char *)outputfile;
    command[count+3] = NULL;

	pid_t child_pid = fork();
	int exec_retval;
	if (child_pid == 0)
	{
		exec_retval = execv(command[0], &command[1]);
		if (exec_retval == -1)
			exit(1);
	}
	
	int child_status;
	pid_t waited_pid = waitpid(child_pid, &child_status, 0);
	if (waited_pid == -1 || waited_pid != child_pid || WEXITSTATUS(child_status) != 0)
		is_succeed = false;

    va_end(args);
    return is_succeed;


/*
 * TODO
 *   Call execv, but first using https://stackoverflow.com/a/13784315/1446624 as a refernce,
 *   redirect standard out to a file specified by outputfile.
 *   The rest of the behaviour is same as do_exec()
 *
*/
}

