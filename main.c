#include<stdio.h>
#include"shell.h"

int main()
{
    system("clear");
    char prompt_string[] = "minishell$";
    char input_string[100];
    
    scan_input(prompt_string,input_string,100);
    
    return 0;
}