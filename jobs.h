#ifndef JOBS_H
#define JOBS_H

#define MAX_JOBS      100

#include<sys/types.h>

typedef enum 
{
    STOP,
    RUNNING
}State;

typedef struct 
{
    int job_id;
    pid_t job_pid;
    State job_state;
    char command[100];
}Job;

extern Job jobs[MAX_JOBS] ;
extern int job_count ;
extern int next_job_id ;
extern int background;

int add_job(pid_t pid,const char *cmd,State state,Job jobs[]);
int find_job_by_pid(pid_t pid,Job jobs[]);
int find_job_by_id(int id,Job jobs[]);
void print_jobs(Job jobs[]);
void execute_bg(const char *cmd,Job jobs[]);

#endif


