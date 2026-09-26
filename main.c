#include<stdio.h>
#include"shell.h"

int main()
{
    system("clear");
    char prompt_string[] = "minishell$";
    char input_string[50];

    scan_input(prompt_string,input_string);
    
    return 0;
}