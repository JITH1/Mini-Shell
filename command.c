#include<stdio.h>
#include"shell.h"

char path[100];
char *external_commands[154];

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
            
        }

    }
}

int extract_external_commands(char **ext)
{
    FILE *ptr = fopen("ext_commands.txt","r");

    if(ptr == NULL)
    {
        printf(RED"\nCan't import external commands...!\n"RESET);
        return 0;
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

    return 1;
}

void print_path(char *prompt,char *path)
{
    printf(GREEN"%s"RESET,prompt);
    printf(BLUE"~%s$ "RESET,path);

}
