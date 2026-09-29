#include<stdio.h>
#include"shell.h"

char path[100];
char *external_commands[154];
int status ;

void scan_input(char *prompt, char *input_string)
{
    
    if(!extract_external_commands(external_commands)) // Export the external commands stored in the file
    {
        return ;
    } 

    if(chdir("..") == 0)
    {
        if(getcwd(path,100))
        {
            //  printf("\nPWD : %s\n",path);
        }
    } 

    while(1)
    {
        print_path(prompt,path);  
        scanf(" %49[^\n]",input_string);  // Enter the Command 

        if(strncmp(input_string,"PS1=",4) == 0) // If present
        {
            if(input_string[4] == ' ')
            {
                printf(RED"\nMini_Shell : %s : Command not found\n"RESET,input_string);
                return ;
            } 

            strcpy(prompt,input_string+4);
        }
        else
        {
            char *cmd = get_command(input_string);    // Get command from the arguments 

            int type = cmd_type(cmd);                // Check the command type 
            
            if(type == BUILTIN)
            {
                execute_internal_commands(cmd); 
            }
            else if(type == EXTERN)
            {
                
            }
            else if(type == INVALID)
            {
                printf("\n%s : command not found\n",cmd);
            }

        }

    }
}

int extract_external_commands(char **ext)
{
    FILE *ptr = fopen("ext_commands.txt","r");

    if(ptr == NULL)
    {
        printf(RED"\nCan't import external commands...!\n"RESET);
        return FAILED;
    }

    int i = 0;
    char buffer[20];

    while(fgets(buffer,sizeof(buffer),ptr) != NULL)
    {
        buffer[strlen(buffer)-1] = '\0';
        *(ext+i) = malloc(sizeof(char)*sizeof(buffer));
        strcpy(*(ext+i),buffer);
        i++; 
    }

    fclose(ptr);

    *(ext+i) = NULL;

    return SUCCESS;
}

void print_path(char *prompt,char *path)
{
    printf(GREEN"%s"RESET,prompt);
    printf(BLUE"~%s$ "RESET,path);

}

void execute_internal_commands(char *cmd)
{
     if(strncmp(cmd,"cd",2) == 0)
     {
         char *space = strchr(cmd+3,' ');

         if(space)
         {
             printf("minishell: cd: too many arguments\n");
             return ;
         }
         
         int flag = chdir(cmd+3);

         if(!flag)
         {
             char *ptr = getcwd(path,100); // On successfully changing the direcctory the new path is stored inside the path array
         }
         else
         {
            if(errno == ENOENT)
            {
                printf("minishell: cd: %s: No such file or directory\n",cmd+3);
                return ;
            }
         }
     }
     else if(strncmp(cmd,"pwd",3) == 0)
     {
          char *ptr = getcwd(path,100);
          printf("%s\n",ptr);
     }
     else if(strncmp(cmd,"exit",4) == 0)
     {
        _exit(0);
     }
     else if(strncmp(cmd,"echo $$",7) == 0)
     {
          printf("%d\n",getpid());
     }
     else if(strncmp(cmd,"echo $?",7) == 0)
     {
          printf("%d ",WEXITSTATUS(status)); 
          if(cmd+7 != NULL)
          printf("%s\n",cmd+7);
     } 
     else if(strncmp(cmd,"echo $SHELL",11) == 0)
     {
        char *ch = getenv("SHELL");
        printf("%s\n",ch);
     }  

}