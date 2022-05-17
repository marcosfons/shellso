#ifndef BACKGROUND_JOB_HEADER
#define BACKGROUND_JOB_HEADER


#include "command.h"

#define STATUS_COMMAND_NOT_EXECUTED_YET INT_MIN


typedef struct background_job background_job;
typedef struct background_job {
	char* command;
	int pid;
	int status;
	background_job* next;
} background_job;

typedef struct {
	background_job* next;
} background_jobs;


background_jobs* create_background_jobs();

void add_background_job(background_jobs* jobs, command* cmd, int pid);

void update_background_job_status(background_job* job, int status);

void update_background_job_status_by_pid(background_jobs* jobs, int pid, int status);

int remove_command_by_pid(background_jobs* jobs, int pid);


// Free only one background_job 
void free_background_job(background_job* job);

// Free the whole linked list of background jobs
void free_background_job_chain(background_job* job);

void free_background_jobs(background_jobs* jobs);


#endif
