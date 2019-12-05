#include "ysh.h"

//We'll split the input by these
#define DELIM " \t\r\n"
//One page size
#define MAX_SIZE 4096
#define MAX_LENGTH 100
#define BUILTINS_LENGTH 2

char *builtins[BUILTINS_LENGTH] = {"cd", "chdir"};

int pid;

int main() {
	start();
	return 0;
}
void handler(int signal) {
	if(signal & SIGINT) {
		/*
			If the user hits ^C we kill the
			running process
		*/
		if(pid != getpid())
			kill(pid, SIGKILL);
	}
}

/*
	Run ysh loop
*/
void start() {
	char *line, **args, *user, *host, *path;

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
		//Split input
		args = tokenize(line);
		//Execute the command
		execute(args);
	}
}

/*
	Read a line from stdin and changes the
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
	Splits the each line into an array of tokens
*/
char **tokenize(char *line) {
	int max_size;
	char **tokens;//To store the array of commands
	char *token;//To store each tokens

	/*
		Malloc each element of the array
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

	free(token);

	return tokens;
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
				it hasn't been traced by ptrace
			*/
			pid = waitpid(pid, &status, WUNTRACED);
		}
	}
	return 0;
}