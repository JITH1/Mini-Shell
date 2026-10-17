#include "shell.h"
#include<stdio.h>

int job_count = 0;
int next_job_id = 1;

int add_job(pid_t pid,const char *cmd,State state,Job **Head)
{
    if(job_count >= MAX_JOBS)
    {
        printf("\nThe Maximum job is reached...!\n");
        return FAILED;
    }

    job_count++;

    if(*Head == NULL)
    {
        Job *new = (Job *)malloc(sizeof(Job));
        new->job_id = next_job_id++;
        new->job_pid = pid; 
        new->job_state = state;
        snprintf(new->command,sizeof(new->command),"%s",cmd);
        new->link = NULL;
        *Head = new;
        return new->job_id; 
    }
    
    Job *temp = *Head;

    while(temp->link != NULL)
    {
        temp = temp->link;
    }

    Job *new = (Job *)malloc(sizeof(Job));
    new->job_id = next_job_id++;
    new->job_pid = pid; 
    new->job_state = state;
    snprintf(new->command,sizeof(new->command),"%s",cmd);
    new->link = NULL;
    temp->link = new;

    return new->job_id; 
}

Job* find_job_by_pid(pid_t pid,Job **Head)
{
    for(Job *temp = *Head ; temp != NULL ; temp = temp->link)
    {
        if(pid == temp->job_pid)
        {
            return temp;
        }

    }

    return NULL;
}

Job* find_job_by_id(int id,Job **Head)
{
    for(Job *temp = *Head ; temp != NULL ; temp = temp->link)
    {
        if(id == temp->job_id)
        {
            return temp;
        }
    }

    return NULL;
}

void print_jobs(Job **Head)
{
    for(Job *temp = *Head ; temp != NULL ; temp = temp->link)
    {
        if(temp->link != NULL)
        printf("[%d]- %s\t%s\n",temp->job_id,temp->job_state == RUNNING ? "Running":"Done",temp->command);
        else if(temp->link == NULL)
        printf("[%d]+ %s\t%s\n",temp->job_id,temp->job_state == RUNNING ? "Running":"Done",temp->command);
    }
}

void print_jobs_pid(Job **Head)
{
    for(Job *temp = *Head ; temp != NULL ; temp = temp->link)
    {
        if(temp->link != NULL)
        printf("[%d]- %d %s\t%s\n",temp->job_id,temp->job_pid,temp->job_state == RUNNING ? "Running":"Done",temp->command);
        else if(temp->link == NULL)
        printf("[%d]+ %d %s\t%s\n",temp->job_id,temp->job_pid,temp->job_state == RUNNING? "Running":"Done",temp->command);
    }
}

void print_run_jobs(Job **Head)
{
    for(Job *temp = *Head ; temp != NULL ; temp = temp->link)
    {
        if((temp->link != NULL) && (temp->job_state == RUNNING))
        printf("[%d]- %s\t%s\n",temp->job_id,"Running",temp->command);
        else if((temp->link == NULL) && (temp->job_state == RUNNING))
        printf("[%d]+ %s\t%s\n",temp->job_id,"Running",temp->command);
    }
}

void print_stop_jobs(Job **Head)
{
    for(Job *temp = *Head ; temp != NULL ; temp = temp->link)
    {
        if(temp->job_state == STOP)
        {
            printf("[%d] Stopped\t%s\n",temp->job_id, temp->command);
        }
    }
}

int update_job(pid_t pid,State state,Job **Head)
{
    Job *temp = find_job_by_pid(pid,Head);

    if(temp)
    {
        temp->job_state = state;
        return SUCCESS;
    }

    return FAILED;
}

void remove_job(pid_t pid,Job **Head)
{
    Job *found = find_job_by_pid(pid,Head);

    if(found == NULL)
    {
        return;
    }
   
    Job *temp = *Head;

    if(temp == found)
    {
        *Head = found->link;
        free(found);
        job_count--; 
        return;
    }

    while(temp != NULL && temp->link != found)
    {
        temp = temp->link;
    }

    if(temp == NULL)
    return;

    temp->link = found->link;
    free(found);
    job_count--;

}

void execute_bg(const char *cmd,Job **Head)
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
            int p = add_job(pid,buffer,RUNNING,Head);

            if(p)
            {
                printf("[%d] %d\n",p,pid);
            }
        }
       
    }
    else 
    {
        perror("fork");
    }
    
}

void handle_background_jobs(Job **Head)
{
    pid_t pid;
    int child_status;

    while((pid = waitpid(-1,&status,WNOHANG|WUNTRACED|WCONTINUED) ) > 0)
    {
        if(WIFEXITED(child_status))
        {   
            remove_job(pid, Head);
        }
        else if(WIFSIGNALED(child_status))
        {
            remove_job(pid, Head);
        }
        else if(WIFSTOPPED(child_status))
        {
            update_job(pid, STOP, Head);
        }
        else if(WIFCONTINUED(child_status))
        {
            update_job(pid, RUNNING, Head);
        }
    }
}

