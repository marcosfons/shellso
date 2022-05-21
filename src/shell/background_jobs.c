#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "background_jobs.h"
#include "command.h"


background_jobs* create_background_jobs() {
	background_jobs* jobs = malloc(sizeof(background_jobs));
	jobs->next = NULL;

	return jobs;
}

void add_background_job(background_jobs* jobs, command* cmd, int pid) {
	background_job* job = malloc(sizeof(background_job));

	job->command = strdup(cmd->argv[0]);
	job->pid = pid;
	job->status = STATUS_COMMAND_NOT_EXECUTED_YET;
	job->next = NULL;

	background_job* last_command = jobs->next;
	if (last_command == NULL) {
		jobs->next = job;
		return;
	}

	while (last_command->next != NULL) {
		last_command = last_command->next;
	}
	last_command->next = job;
}

void update_background_job_status(background_job* job, int status) {
	job->status = status;
}

void update_background_job_status_by_pid(background_jobs* jobs, int pid, int status) {
	background_job* job = jobs->next;
	while (job->next != NULL && job->pid != pid) {
		job = job->next;
	}

	if (job != NULL && job->pid == pid) {
		job->status = status;
	}
}

int remove_command_by_pid(background_jobs* jobs, int pid) {
	if (jobs->next != NULL && jobs->next->pid == pid) {
		background_job* next = jobs->next->next;
		free(jobs->next);
		jobs->next = next;
		
		return pid;
	}
	background_job* curr = jobs->next;

	while (curr->next != NULL && curr->next->pid != pid) {
		curr = curr->next;
	}

	if (curr->next != NULL && curr->next->pid == pid) {
		background_job* next = curr->next->next;
		free(curr->next);
		curr->next = next;

		return pid;
	}

	return -1;
}

bool job_running(background_job* job) {
	return job->status == INT_MIN ||
	      (!WIFEXITED(job->status) && !WIFSIGNALED(job->status));
}

void free_background_job(background_job* job) {
	free(job->command);
	free(job);
}

void free_background_job_chain(background_job* job) {
	if (job->next != NULL) {
		free_background_job(job->next);
	}

	free_background_job(job);
}

void free_background_jobs(background_jobs* jobs) {
	if (jobs->next != NULL) {
		free_background_job_chain(jobs->next);
	}

	free(jobs);
}



