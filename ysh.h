/**********************************
 * Author: Emilio E. G. Cantón Bermúdez
 * Date: Nov. 28
 * Last Updated: Dec. 6
 * License: MIT
**********************************/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <wait.h>
#include <signal.h>
#include <fcntl.h>
#include <string.h>	

#define BUFFER_SIZE 1024
//We'll split the input by these
#define DELIM " \t\r\n"
#define PIPE_DELIM '|'
//One page size
#define MAX_SIZE 4096
#define MAX_LENGTH 100
#define BUILTINS_LENGTH 4
#define HELP_STRING "\
----------------------------------------------------\n\
Yet Another Shell by Emilio E. G. Cantón Bermúdez\n\
----------------------------------------------------\n\
Thanks for using YSH! As of today the following are\n\
implemented:\n\
-	Running commands\n\
-	Signal Handling\n\
-	Using environment variables\n\
-	Piping\n\n\
A few extra commands you should know:\n\
-	help: Show this message\n\
-	exit: Exit the shell\n\
"

typedef struct {
	int n_commands;
	//Maximum 100 commands with 100 strings including args
	char *commands[MAX_LENGTH][MAX_LENGTH];
	int pipes[MAX_LENGTH - 1][2];
} pipecmd;

int pid;
char *builtins[BUILTINS_LENGTH] = {"cd", "chdir", "exit", "help"};

char *readline();
void tokenize(char *line);
int execute(char **args, int redirects[2][2]);
int execute_pipes(pipecmd *cmds);
void start();