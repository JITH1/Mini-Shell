#include "shell.h"
#include<stdio.h>

int job_count = 0;
int next_job_id = 1;

int add_job(pid_t pid,const char *cmd,State state,Job jobs[])
{
    if(job_count >= MAX_JOBS)
    {
        printf("\nThe Maximum job is reached...!\n");
        return FAILED;
    }

    int i = job_count++;
    
    jobs[i].job_id = next_job_id++;
    jobs[i].job_pid = pid;
    jobs[i].job_state = state;
    snprintf(jobs[i].command,sizeof(jobs[i].command),"%s",cmd);

    return jobs[i].job_id;
}

int find_job_by_pid(pid_t pid,Job jobs[])
{
    for(int i = 0; i<job_count ; i++)
    {
        if(pid == jobs[i].job_pid)
        {
            return SUCCESS;
        }

    }

    return FAILED;
}

int find_job_by_id(int id,Job jobs[])
{
    for(int i = 0; i<job_count ; i++)
    {
        if(id == jobs[i].job_id)
        {
            return SUCCESS;
        }
    }

    return FAILED;
}

void print_jobs(Job jobs[])
{
    for(int i = 0 ; i<job_count ; i++)
    {
        printf("[%d] %s\t%s\n",jobs[i].job_id,jobs[i].job_state == RUNNING? "Running":"Stopped",jobs[i].command);
    }
}

void execute_bg(const char *cmd,Job jobs[])
{
    char buffer[100];

    snprintf(buffer,sizeof(buffer),"%s",cmd);

    buffer[strcspn(buffer,"&") - 1] = '\0';

    char *commands[20];

    int i = 0 , row = 0 , col = 0;
     
    int space = 0 , index = 0;

    char temp[15][25];

    while(buffer[i] != '\0' && row < 15)
    {
         if(buffer[i] != ' ')
         {
            if(space)
            space = 0;

            if(col < 25)
            temp[row][col++] = buffer[i]; 
         }
         else if(buffer[i] == ' ' && (space == 0))
         {
            temp[row][col] = '\0';
            commands[index] = temp[row];
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
        commands[index] = temp[row];
        index++;
    }

    commands[index] = NULL ; 

    pid_t pid = fork();

    if(pid == 0)
    {
        execvp(commands[0],commands);
        perror("Failed ");
        _exit(127); 
    }
    else if(pid > 0)
    {
        if(background)
        {
            int p = add_job(pid,buffer,RUNNING,jobs);

            if(p)
            {
                printf("[%d] %d\n",p,pid);
            }
        }
        else
        {
            int status;

            while(waitpid(pid,&status,WUNTRACED) == -1)
            { 
                 if(errno == EINTR)
                 {
                    continue;
                 }
            }  

            if(WIFEXITED(status))
            {
                int id = add_job(pid,buffer,STOP,jobs);

                if(id)
                printf("[%d]+ Stopped\t%s\n",id,buffer);
            }
        }
    }
    else 
    {
        perror("fork");
    }

    
}

