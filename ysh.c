/**********************************
 * Author: Emilio E. G. Cantón Bermúdez
 * Date: Nov. 28
 * Last Updated: Dec. 6
 * License: MIT
**********************************/
#include "ysh.h"

int main() {
	start();
	return 0;
}

/*
	Handle signals
*/
void handler(int signal) {
	if(signal & SIGINT) {
		/*
			If the user hits ^C we kill the
			running process
		*/
		if (pid != getpid()) {
			kill(pid, SIGKILL);
		}
	}
}

/*
	Run ysh loop
*/
void start() {
	char *line, *user, *host, *path;

	//Catch SIGINT
	signal(SIGINT, handler);

	//Show context to the user
	user = getenv("USER");
	host = getenv("HOSTNAME");
	path = malloc(MAX_LENGTH);

	while(1) {
		/*
			Get the cwd and show it to the user
		*/
		if(getcwd(path, MAX_LENGTH) == NULL) {
			perror("Could not get path");
		}
		/*
			Get the current pid in order to avoid
			breaking the shell instead of the running
			process on SIGINT.
		*/
		pid = getpid();
		printf("%s@%s:%s>", user, host, path);
		//Read input
		line = readline();
		//Split input and execute it
		tokenize(line);
	}
}

/*
	Reads a line from stdin and changes the
	\n character to \0.
*/
char *readline() {
	char *line;
	size_t max_length;

	max_length = MAX_LENGTH;
	//Malloc memory for the input
	line = malloc(max_length);
	//Get the input from the user
	if(getline(&line, &max_length, stdin) == -1) {
		perror("Error while reading line");
	}

	return line;
}

/*
	Splits a line into an array of tokens split by DELIM
*/
void tokenize(char *line) {
	int max_size;
	char **tokens;//To store the array of commands
	char *token;//To store each tokens


	/*
		Malloc the array
		Malloc the token (buffer)
	*/
	max_size = MAX_SIZE;
	tokens = malloc(max_size * sizeof(char *));
	token = malloc(MAX_LENGTH);
	/*
		As specified by the man 3 of strtok it returns a pointer
		to the token or NULL if no tokens are available anymore.
	*/
	int i = 0;//counter
	strcpy(token, line);
	token = strtok(token, DELIM);

	while(token != NULL) {
		int str_len = strlen(token);
		//Malloc memory to the new string in the array
		tokens[i] = malloc(str_len + 1);

		//Check if the token is an environment variable
		if(token[0] == '$') {
			char *variable;

			variable = malloc(MAX_LENGTH);
			strncpy(variable, token + 1, MAX_LENGTH);
			//Check if the environment variable exists
			if((token = getenv(variable)) == NULL) {
				//Else assign an empty string
				token = "";
			}
		}

		//Copy the token into the array
		strncpy(tokens[i++], token, str_len + 1);
		//If the size of the input surpassed the limit
		if(i >= MAX_SIZE) {
			//Reallocate more memory to the array
			max_size += MAX_SIZE;
			tokens = realloc(tokens, max_size * sizeof(char *));
		}
		//Read the next token
		token = strtok(NULL, DELIM);
	}
	/*
		When we exit the last token read we set this string as
		NULL as specified by the man of execvp
	*/
	tokens[i] = NULL;
	/*
		If there's a PIPE_DELIM ('|') on the string
		save the whole pipeline of commands and run it
	*/
	if(strchr(line, PIPE_DELIM) != NULL) {
		pipecmd *cmds;//Where the commands and pipes will be saved


		cmds = malloc(sizeof(pipecmd));//Malloc struct into heap
		cmds->n_commands = 1;//Because at least one command is running

		i = 0;//Row (Determines each command)
		int j = 0;//Column (Determines the argument count of each command)
		int counter = 0;//Determines the position in the whole input string by user
		int pipes[2];//To temporarily save the pipe fds
		//If we haven't reached the end of the pipeline
		while(tokens[counter] != NULL) {
			//Check if the current string is a PIPE_DELIM
			if(strncmp(tokens[counter], "|", 1) == 0) {
				//Adding a new commands to the list
				cmds->n_commands++;
				//Set the last string as NULL for execvp requirements
				cmds->commands[i][j + 1] = NULL;
				//Create a new pipe
				pipe(pipes);
				//Save the read fd to the next process
				cmds->pipes[i + 1][0] = pipes[0];
				//Save the write fd to the current process
				cmds->pipes[i][1] = pipes[1];
				//Next command
				i++;
				//Beginning of command
				j = 0;
				//To avoid reading PIPE_DELIM again
				counter++;
			}
			//Simply add the command or args to the corresponding ith value
			cmds->commands[i][j++] = tokens[counter++];
		}
		//Execute the pipeline
		execute_pipes(cmds);
	} else {
		//No pipeline, just execute the command
		free(token);
		execute(tokens);
	}
}

/*
	Executes a tokenized array of char pointers
*/
int execute(char **args) {
	int status, builtin;

	builtin = 0;
	/*
		Check if the command given is a builtin in order
		to execute it directly and not on the child (fork)
	*/
	for(int i = 0; i < BUILTINS_LENGTH; i++) {
		if(strncmp(builtins[i], args[0], strlen(builtins[i])) == 0) {
			builtin = 1;
			switch (i)
			{
			case 0://cd
			case 1://chdir
				chdir(args[1]);
				return 0;
			case 2://exit
				printf("Thanks for using YSH!\n");
				exit(0);
				break;
			case 3://help
				printf("%s\n", HELP_STRING);
				break;
			default:
				break;
			}
		}
	}

	if(!builtin) {
		//Fork the execution into another process
		if(!(pid = fork())) {
			//Execute the input from user
			execvp(args[0], args);
		} else {
			/*
				Wait for the child process and return even if
				it hasn't been traced by ptrace (specified on man of waitpid)
			*/
			pid = waitpid(pid, &status, WUNTRACED);
		}
	}
	return 0;
}

/*
	Executes a number of piped commands
*/
int execute_pipes(pipecmd *cmds) {
	//For each commands specified
	for (int i = 0; i < cmds->n_commands; i++) {
		//Create a new process to execute the process
		if (!(pid = fork())) {
			//If the process is not the first one
			if(i > 0) {
				//Duplicate the input file descriptor (same as below, but w/ stdin)
				dup2(cmds->pipes[i][0], STDIN_FILENO);
				//And close the output file descriptor 
				close(cmds->pipes[i - 1][1]);
			}
			//If the process is not the last one
			if(i < cmds->n_commands - 1) {
				/*
					As I better understand it... The pipe fd
					will now be pointing to the stdout fd position
					on the fds table of this process, therefore
					writing everything to the pipe.
				*/
				dup2(cmds->pipes[i][1], STDOUT_FILENO);
				///Close the input file descriptor of the next process
				close(cmds->pipes[i + 1][0]);
			}
			execvp(cmds->commands[i][0], cmds->commands[i]);
		} else {
			int status;
			/*
				Close each of the pipe ends on 
				the parent in order to avoid stalling
				and to avoid each subsquent process
				to have n_commands - 1 * 2 fds instead
				of just 2.
			*/
			if(cmds->pipes[i][0] != 0) {
				close(cmds->pipes[i][0]);
			}
			if(cmds->pipes[i][1] != 0) {
				close(cmds->pipes[i][1]);
			}
			//Wait for the last child that executed
			waitpid(pid, &status, WUNTRACED);
		}
	}
	return 0;
}