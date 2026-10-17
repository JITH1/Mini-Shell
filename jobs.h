#ifndef JOBS_H
#define JOBS_H

#define MAX_JOBS      100

#include<sys/types.h>

typedef enum 
{
    STOP,
    RUNNING
}State;

typedef struct node
{
    int job_id;
    pid_t job_pid;
    State job_state;
    char command[100];
    struct node *link;
}Job;

extern int job_count ;
extern int next_job_id ;
extern int background;
extern Job *Head;
extern int status ;

int add_job(pid_t pid,const char *cmd,State state,Job **Head);
int update_job(pid_t pid,State state,Job **Head);
void remove_job(pid_t pid,Job **Head);
Job* find_job_by_pid(pid_t pid,Job **Head);
Job* find_job_by_id(int id,Job **Head);
void print_jobs(Job **Head);
void print_jobs_pid(Job **Head);
void print_run_jobs(Job **Head);
void print_stop_jobs(Job **Head);
void execute_bg(const char *cmd,Job **Head);
void handle_background_jobs(Job **Head);

#endif


