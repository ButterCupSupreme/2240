/*************************************************************************\
|                     Assignment 3: rhelix (Ryan's Shell)                 |
|Name: Ryan DePrekel                                                      |
|Due:                                                                     |
|Class: CS2240                                                            |
|                                                                         |
|        ᘓᘓᘓᘓᘓᘓᘓᘓ                                                        |
|      rhelix_loop() - read arguments, parse line, execute                |
|      need to fork, pipe, dup, exec                                      |
|                                                                         |
|      rhelix_exec() - looks for built ins then sends the args to launch  |
|                                                                         |
|      rhelix_launch() - all the magic is here. execs commands with pipes |
|      and redirects (output only!)                                       |
|                                                                         |
|      rhelix_loop() -> rhelix_exec() -> rhelix_launch                    |
|      Built-Ins                                                          |
|        exit, cd, and kill -9                                            |
|                                                                         |
| http://tinyurl.com/kmzdzud where I got most of my help to get started   |
|                                                                         |
|                                                                         |
\*************************************************************************/
#include "rhelix.h"

int main (int argc, char **argv)
{
	start_up();

	// Shell loop 
	rhelix_loop();

	return EXIT_SUCCESS;
}
/*
	Displays the start up file 
	Praise Helix!
*/
void start_up()
{
	FILE *fd;
	char line[80];

	fd = fopen("omanyte.txt", "rt");

	if(fd == NULL)
	{
		err_sys("Failed open!");
	}
	printf("\n");
	
	while(fgets(line, 80, fd) != NULL)
	{
		printf("%s", line);
	}
	printf("\n");
	fclose(fd);
}


void rhelix_loop(void)
{
	char *line;
	char **args;
	char buff[PATH_MAX + 1];
	char *cwd;
	int  status = 1;
	int  numArgs;

	while(status)
	{
		cwd = getcwd(buff, PATH_MAX + 1);  //Displays current working directory
		if(cwd != NULL)
		{
			printf("%s ᘓ~ ", cwd);
		}
		else
		{
			printf(" ᘓ~");
		}
		line = rhelix_read_line();
		numArgs = makeargv(line, "|", &(args)); //Split on pipes
		status = rhelix_exec(args);

		free(line);
		free(args);
	}
}


/*
	This is where it checks for built-ins in the commands
	if not, send it to launch where most of the fun happens

*/
int rhelix_exec(char **args)
{
	int i, cmdCount;
	char **cmd;

	cmdCount = makeargv(args[0], " ", &cmd);

	if (args[0] == NULL)
	{
		//This was an empty command.
		return 1;
	}

	// Check for a built-in
	if(strcmp(cmd[0], "cd") == 0)
	{
		rhelix_cd(cmd);
	}
	else if (strcmp(cmd[0], "kill") == 0)
	{
		rhelix_kill(cmd, cmdCount);
	}
	else if (strcmp(cmd[0], "exit") == 0)
	{
		rhelix_exit(cmd);
	}
	else if (strcmp(cmd[0], "helix") == 0)
	{
		rhelix_helix(cmd);
	}
	else // if not a built-in run exec command
	{
		return rhelix_launch(args);
	}
}

/*
	Launch will execute commands.
	This is where I lost my head on pipes. Don't think I will have enough 
	time for redirects :'(
	Number of thunks definitely > lines of code
*/

int rhelix_launch(char **args)
{
	pid_t pid, wpid;
	int status, i, fd[2], in, j;
	int saveIn = dup(0);
	int saveOut = dup(1);
	int argscount = 0;
	int newArgCount = 0;
	char **newArgs;  

	while(args[argscount]) // Count number of args
	{
		argscount++;
	}
	
	in = 0;

	for (i = 0; i < argscount - 1; ++i)
	{
		//Splits first part of argument on white space 
		newArgCount = makeargv(args[i], " ", &(newArgs));
		/* Not sure when to look for "<"
		probably need to reorganize the for loop controlling the whole thing*/
		for(j = 0; j < newArgCount; j++)
		{
			if(strcmp(newArgs[j], "<") == 0)
			{
				input_redirect(newArgs, newArgCount);
			}
			
		}

		//start plumbing  ***JOY!*** This is where my head started hurting

		if (pipe(fd) == -1)
		{
			err_sys("rhelix: pipe error");
			exit(EXIT_FAILURE);
		}

		spawn(in, fd[1], newArgs);  //Spawns a child with plumbing

		close(fd[1]); // Close write end of the pipe.
		in = fd[0];   // Keep read end.
	}

	if (in != 0)
	{
		dup2(in, 0);
	}

	newArgCount = makeargv(args[i], " ", &(newArgs));
	
	for(i = 0; i < newArgCount; i++)
	{
		if(strcmp(newArgs[i], ">") == 0)
		{
			output_redirect(newArgs, newArgCount);
		}
		else if(strcmp(newArgs[i], "<") == 0)
		{
			input_redirect(newArgs, newArgCount);
		}
	}

	//check for backgrounding
	if(strcmp(newArgs[newArgCount - 1], "&") == 0)
	{
		
		status = rhelix_background(newArgs);
	}
	else //exec the last command
	{
		pid = fork();
		if (pid == 0)
		{
			if (execvp(newArgs[0], newArgs) == -1)
			{
				err_sys("rhelix: failed running command");
			}
			exit(EXIT_FAILURE);
		}
		else if (pid < 0)
		{
			err_sys("rhelix: Bad Fork!");
		}
		else
		{
			do
			{
				wpid = waitpid(pid, &status, WUNTRACED);
			} while (!WIFEXITED(status) && !WIFSIGNALED(status));
		}
	}
	dup2(saveIn, 0);
	close(saveIn);
	dup2(saveOut, 1);
	close(saveOut);	
	return 1;
}

int rhelix_helix(char **args)
{
	start_up();
	return 1;
}


/*
	Change Directory implementing like in bash. *mostly*
*/
int rhelix_cd(char **args)
{
	if (args[1] == NULL)
	{
		chdir(getenv("HOME"));
	}
	else 
	{
		if(chdir(args[1]) != 0)
		{
			fprintf(stderr,"rhelix: cd error\n");
		}
	}
	return 1;
}
/*
	Kill built-in
		-will kill a process
		-will kill -9 a process
*/

int rhelix_kill(char **args, int argsCount)
{
	pid_t process;    //varible used to keep track of pid
	int i;
	
	
	
	if(argsCount < 2) // case when its "kill"
	{
		fprintf(stderr, "Not enough arguments!\n");
	}
	else              // cases when its a valid kill command
	{
		if(strcmp(args[1],"-9") == 0)
		{
			process = atoi(args[2]);
			if(kill(process, SIGKILL) != 0)
			{
				fprintf(stderr, "Not a valid pid\n");
				return 1;
			}
		}
		else
		{
			process = atoi(args[1]);
			if (kill(process, SIGTERM) != 0)
			{
				fprintf(stderr, "Not a valid pid\n");
				return 1;
			}
		}
		printf("TERMINATED pid %d\n", process); // Response to STDIN
	}
	return 1;
}


int rhelix_exit(char **args)
{
	return 0;
}


/*
	does the same as executing one command, but doesn't wait.
	Note: tends to create zombie process
	Possible fix; send SIGCHLD signal back to parent process [DONE] Thanks Colin!
*/ 
int rhelix_background(char **args)
{
	pid_t pid;
	int argscount = 0;
	int i;
	while(args[argscount])
	{
		argscount++;
	} 
	args[argscount - 1] = NULL;

	signal(SIGCHLD, SIG_IGN);  // Ignore child fate, kill those zombies, *BANG BANG*
	pid = fork();
	if(pid == 0) //Child Process
	{
		if(execvp(args[0], args) == -1)
		{
			err_sys("rhelix: Failed Exec!");
		}
		exit(EXIT_FAILURE);
	}
	else if(pid < 0) // Bad fork
	{
		err_sys("rhelix: Bad Fork!");
	}

	return 1;
}

/*
	Using getline() to get the line in a efficent way.
*/

char *rhelix_read_line(void)
{
	char *line = NULL;
	ssize_t bufsize = 0;
	getline(&line, &bufsize, stdin);
	line = strip_new_line(line);
	return line;
}

/*
	Removes the new line character at the end of a line
*/
char *strip_new_line(char* inLine)
{
	char *nLine = malloc(strlen(inLine) + 1);
	if(nLine)
	{
		char *nLine2 = nLine;
		while(*inLine != '\0')
		{
			if(*inLine != '\n' && *inLine != '\t')
			{
				*nLine2++ = *inLine++;
			}
			else 
			{
				++inLine;
			}
		}
		*nLine2 = '\0';
	}
	return nLine;
}
/*
	Found some code on stackoverflow that helped me out 
	http://stackoverflow.com/questions/8082932/connecting-n-commands-with-pipes-in-a-shell
	Helper function for pipes...needed googleland for this. Thanks stackOverflow!
*/
int spawn(int in, int out, char **args)
{
	pid_t pid, wpid;
	int status;
	if((pid = fork()) == 0)
	{
		if(in != 0)
		{
			dup2(in, 0);
			close(in);
		}
		if(out != 1)
		{
			dup2(out, 1);
			close(out);
		}

		return execvp(args[0], args);
	}
	else
	{
		wait(NULL);
	}
	return pid;
}

/*
	Output redirection:
	handles the redirection to the right
	open file descriptor
	dup stdout to fd 

	Note to self: right now I think it is trying to exec the whole line
	i.e. if "ls > file" it trying to exec this whole line instead of just "ls"
	TODO: Fix above error! [FIXED]
	
	Doesn't seem to break the piping though :D

	Works with pipes!

*/

int output_redirect(char **args, int argsCount)
{
	int fd, i;

	//Create the file for redirecting the output to
	
	
	for(i = 0; i < argsCount; i++)
	{
		if(strcmp(args[i], ">") == 0)
		{
			args[i] = NULL;
			fd = open(args[i+1],  O_WRONLY | O_CREAT | O_TRUNC,  S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
		}
	}

	if(fd < 0)
	{
		fprintf(stderr, "Failed open output\n");
	}

	//dup stdout to fd
	dup2(fd, 1);

	return 1;
}


/*
	Input Redirection
	works with pipes only currently 
*/
int input_redirect(char **args, int argsCount)
{
	int fd, i;

	fd = open(args[argsCount-1],O_RDONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	
	for(i = 0; i < argsCount; i++)
	{
		if(strcmp(args[i], "<") == 0)
		{
			args[i] = NULL;		
		}
	}

	if (fd < 0)
	{
		fprintf(stderr, "Failed open input\n");
		return 1;
	}

    // dup in to file
	dup2(fd, 0);

	return 1;
}






