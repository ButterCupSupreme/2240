
#include "apue.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <errno.h>
#include <signal.h>

#define BUFF 2048 
#define PATH_MAX 128
#define FALSE 0
#define TRUE 1

void start_up();
void rhelix_loop(void);
char *rhelix_read_line();
int makeargv(char *s, char *delimiters, char ***argvp); 
int rhelix_exec(char **args);
int rhelix_num_builtins();
int rhelix_cd(char **args);
int rhelix_kill(char **args, int argsCount);
int rhelix_helix(char **args);
int rhelix_exit(char **args);
char *strip_new_line(char *inLine);
int rhelix_background(char **args);
int spawn(int in, int out, char **args);
int output_redirect(char **args, int argsCount);
int input_redirect(char **args, int argsCount);