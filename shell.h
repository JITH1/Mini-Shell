#ifndef SHELL_H
#define SHELL_H

#include<fcntl.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>

#define RED         "\x1b[31m"
#define GREEN       "\x1b[32m"
#define YELLOW      "\x1b[33m"
#define BLUE        "\x1b[34m"
#define RESET       "\x1b[0m"

extern char *external_commands[154];
extern char path[100];

void scan_input(char *prompt, char *input_string);
void print_path(char *prompt,char *path);

int extract_external_commands(char **ext); 

#endif