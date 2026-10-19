#include<stdio.h>
#include "shell.h"

int validate_pipe(char *cmd)
{
    int pipe_count = 0;
    int total_cmd = 1;

    for (int i = 0; cmd[i] != '\0'; i++)
    {
        if (cmd[i] == '|')
        {
            pipe_count++;
            total_cmd++;
        }
    }

    return ((pipe_count > 0) && (total_cmd > 1)) ? SUCCESS : FAILED ;
}

void execute_pipe(char *cmd)
{
    char *commands[100];
    char *argv[100][50];

    int pipe_count = 0;
    int total_cmd = 1;

    for (int i = 0; cmd[i] != '\0'; i++)
    {
        if (cmd[i] == '|')
        {
            pipe_count++;
            total_cmd++;
        }
    }

    if (total_cmd > 100)
    {
        fprintf(stderr, "Too many commands\n");
        return;
    }

    int index = 0;
    commands[index++] = cmd;

    for (int i = 0; cmd[i] != '\0'; i++)
    {
        if (cmd[i] == '|')
        {
            cmd[i] = '\0';
            commands[index++] = &cmd[i + 1];
        }
    }

    for (int i = 0; i < total_cmd; i++)
    {
        char *p = commands[i];

        while (*p == ' ' || *p == '\t')
            p++;

        int argc = 0;
        char *token = strtok(p, " \t\n");

        while (token != NULL && argc < 49)
        {
            argv[i][argc++] = token;
            token = strtok(NULL, " \t\n");
        }

        argv[i][argc] = NULL;

        if (argc == 0)
        {
            fprintf(stderr, "Empty command\n");
            return;
        }
    }

    int fd[99][2];

    for (int i = 0; i < pipe_count; i++)
    {
        if (pipe(fd[i]) == -1)
        {
            perror("pipe");
            return;
        }
    }

    for (int i = 0; i < total_cmd; i++)
    {
        pid_t pid = fork();

        if (pid == -1)
        {
            perror("fork");
            break;
        }

        if (pid == 0)
        {
    
            if(i > 0)
            dup2(fd[i - 1][0], STDIN_FILENO);

            if(i < pipe_count)
                dup2(fd[i][1], STDOUT_FILENO);

            for(int j = 0; j < pipe_count; j++)
            {
                close(fd[j][0]);
                close(fd[j][1]);
            }

            execvp(argv[i][0], argv[i]);

            perror("execvp");
            _exit(127);
        }

    }

    for (int i = 0; i < pipe_count; i++)
    {
        close(fd[i][0]);
        close(fd[i][1]);
    }

}
