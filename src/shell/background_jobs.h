#ifndef BACKGROUND_JOB_HEADER
#define BACKGROUND_JOB_HEADER


#include "command.h"


typedef struct {
	command* next;
} background_jobs;


background_jobs* create_background_jobs(command* first_command);

void add_command_chain(background_jobs* jobs, command* command_chain);

int update_command_by_pid(background_jobs* jobs, int pid, int status);

int remove_command_by_pid(background_jobs* jobs, int pid);

void free_background_jobs(background_jobs* jobs);


#endif
