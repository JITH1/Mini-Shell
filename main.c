#include<stdio.h>
#include"shell.h"

 char prompt_string[] = "minishell$";

int main()
{
    system("clear");
    char input_string[100];
    
    scan_input(prompt_string,input_string,100);
    
    return 0;
}