#ifndef SHELL_H
#define SHELL_H

#include "jobs.h"

#include<fcntl.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<sys/wait.h>
#include<signal.h>

#define RED         "\x1b[1;31m"
#define GREEN       "\x1b[1;32m"
#define YELLOW      "\x1b[1;33m"
#define BLUE        "\x1b[1;34m"
#define RESET       "\x1b[0m"

#define BUILTIN         1
#define EXTERN          2
#define INVALID         3 

#define SUCCESS         1
#define FAILED          0   

extern char *external_commands[154];
extern char path[100];
extern char *builtins[];
extern char get_cmd[20];

void scan_input(char *prompt, char *input_string,size_t size);
void print_path(char *prompt,char *path);
char* get_command(char *cmd);
int cmd_type(char *cmd);

int extract_external_commands(char **ext); 
void execute_internal_commands(char *cmd);
void execute_external_commands(char *cmd);

#endif