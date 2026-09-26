#include<stdio.h>
#include "shell.h"

char get_cmd[20];

char *builtins[] = {"jobs","echo", "printf", "read", "cd", "pwd", "pushd", "popd", "dirs", "let", "eval",
						"set", "unset", "export", "declare", "typeset", "readonly", "getopts", "source",
						"exit", "exec", "shopt", "caller", "true", "type", "hash", "bind", "help", NULL};


char* get_command(char *cmd)
{
    int i = 0 , j = 0 ;
    
    while(cmd[i] != ' ' && cmd[i]!='\0')
    {
        get_cmd[j++] = cmd[i];
        i++;
    }

    get_cmd[j] = '\0' ;

    return get_cmd ;  // Get the first word from the arguments 
    
}

int cmd_type(char *cmd)
{
    int i = 0 ;

    while(builtins[i] != NULL)
    {
         if(strcmp(builtins[i],cmd) == 0)
         return BUILTIN ;   

         i++;
    }

    i = 0;

    while(*(external_commands+i) != NULL)
    {
        if(strcmp(*(external_commands+i),cmd) == 0)
        return EXTERN;

        i++;
    }

    return INVALID ;

}
