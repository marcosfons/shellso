#include <stdlib.h>
#include <assert.h>

#include "background_jobs.h"
#include "command.h"


background_jobs* create_background_jobs(command* first_command) {
	background_jobs* jobs = malloc(sizeof(background_jobs));
	jobs->next = first_command;

	return jobs;
}

void add_command_chain(background_jobs* jobs, command* command_chain) {
	command* last_command = jobs->next;

	while (last_command != NULL) {
		last_command = last_command->next;
	}

	last_command->next = command_chain;
}

int update_command_by_pid(background_jobs* jobs, int pid, int status) {
	command* curr = jobs->next;
	while (curr != NULL && curr->pid != pid) {
		curr = curr->next;
	}
	
	if (curr != NULL) {
		assert(curr->pid == PID_COMMAND_NOT_EXECUTED_YET);
		assert(curr->status == STATUS_COMMAND_NOT_EXECUTED_YET);

		curr->pid = pid;
		curr->status = status;

		return status;
	}

	return -1;
}

int remove_command_by_pid(background_jobs* jobs, int pid) {
	if (jobs->next != NULL && jobs->next->pid == pid) {
		command* next = jobs->next->next;
		free(jobs->next);
		jobs->next = next;
		
		return pid;
	}
	command* curr = jobs->next;

	while (curr->next != NULL && curr->next->pid != pid) {
		curr = curr->next;
	}

	if (curr->next != NULL && curr->next->pid == pid) {
		command* next = curr->next->next;
		free(curr->next);
		curr->next = next;

		return pid;
	}

	return -1;
}

void free_background_jobs(background_jobs* jobs) {
	if (jobs->next != NULL) {
		command_free(jobs->next);
	}

	free(jobs);
}



