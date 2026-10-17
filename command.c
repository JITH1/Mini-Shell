#include<stdio.h>
#include"shell.h"

char path[100];
char *external_commands[154];
int status ;
int background = 0;

void scan_input(char *prompt, char *input_string,size_t size)
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

    Job *Head = NULL;

    while(1)
    {

        handle_background_jobs(&Head);

        print_path(prompt,path);  

        if(fgets(input_string,size,stdin) == NULL)  // Enter the Command 
        {
             break ;
        }  

        input_string[strcspn(input_string,"\n")] = '\0';

        size_t len = strlen(input_string);

        background = 0;

        if(len > 0 && input_string[len-1] == '&')
        {
            background = 1;
            execute_bg(input_string,&Head);
            continue;
        }

        if(strncmp(input_string,"PS1=",4) == 0) // If present
        {
            if(input_string[4] == ' ')
            {
                printf(RED"\nMini_Shell : %s : Command not found\n"RESET,input_string);
                return ;
            } 

            strcpy(prompt,input_string+4);
        }
        else if(strcmp(input_string,"jobs") == 0)
        {
            print_jobs(&Head);    
        }
        else if(strcmp(input_string,"jobs -l") == 0)
        {
             print_jobs_pid(&Head);
        }
        else if(strcmp(input_string,"jobs -r") == 0)
        {
            print_run_jobs(&Head);
        }
        else if(strcmp(input_string,"jobs -s") == 0)
        {
            print_stop_jobs(&Head);
        }
        else
        {
            char *cmd = get_command(input_string);    // Get command from the arguments 

            int type = cmd_type(cmd);                // Check the command type 
            
            if(type == BUILTIN)
            {
                execute_internal_commands(input_string); 
            }
            else if(type == EXTERN)
            {
                execute_external_commands(input_string);
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

             if(ptr == NULL)
             {
                return ;
             }
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

          if(ptr)
          printf("%s\n",ptr);
          else
          perror("pwd");

     }
     else if(strncmp(cmd,"exit",4) == 0)
     {
         exit(0);
     }
     else if(strncmp(cmd,"echo $$",7) == 0)
     {
          printf("%d\n",getpid());
     }
     else if(strncmp(cmd,"echo $?",7) == 0)
     {
          if(WIFEXITED(status))
          {
              printf("%d \n",WEXITSTATUS(status)); 
          }
          else if(WIFSIGNALED(status))
          {
              printf("%d\n",128+WTERMSIG(status));
          }
     } 
     else if(strncmp(cmd,"echo $SHELL",11) == 0)
     {
        char *ch = getenv("SHELL");

        if(ch)
        printf("%s\n",ch);
        else
        printf("\n");

     }  

}

void execute_external_commands(char *cmd)
{
     char *buffer[15];

     int i = 0 , row = 0 , col = 0;
     
     int space = 0 , index = 0;

     char temp[15][25];

     while(cmd[i] != '\0' && row < 15)
     {
         if(cmd[i] != ' ')
         {
            if(space)
            space = 0;

            if(col < 25)
            temp[row][col++] = cmd[i]; 
         }
         else if(cmd[i] == ' ' && (space == 0))
         {
            temp[row][col] = '\0';
            buffer[index] = temp[row];
            index++;
            row++ ;
            col = 0;
            space = 1;
         }

         i++;
     }

     if(col > 0 && col < 25)
     {
        temp[row][col] = '\0';
        buffer[index] = temp[row];
        index++;
     }

     buffer[index] = NULL ; 

     i = 0;

     pid_t pid = fork();

     if(pid == 0)
     {
        execvp(buffer[0],buffer); 
        perror("Failed ");
        exit(1);
     }
     else if(pid > 0)
     {
        waitpid(pid,NULL,WUNTRACED);
     }
     else if(pid == -1)
     {
        printf("\nFailed to execute process...!\n");
     }
 
}

