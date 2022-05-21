#ifndef BACKGROUND_JOB_HEADER
#define BACKGROUND_JOB_HEADER

#include <stdbool.h>

#include "command.h"

#define STATUS_COMMAND_NOT_EXECUTED_YET INT_MIN


typedef struct background_job background_job;

/** @brief Linked list item of job that is in execution */
typedef struct background_job {
	char* command;        /**< The name of the command that are in execution */
	int pid;              /**< The process id of the executing program */
	int status;           /**< The status of the running process */
	background_job* next; /**< Points to the next item in the linked list */
} background_job;

/** @brief Linked list of jobs that are in execution, had item */
typedef struct {
	background_job* next;
} background_jobs;


/// @brief Creates a new background_jobs linked list
/// @return Returns a pointer to the newly created list
background_jobs* create_background_jobs();

/// @brief Add a running job to the list
///
/// @param jobs The list of running jobs in the shell
/// @param cmd The command that is running
/// @param pid The process id of the running command
void add_background_job(background_jobs* jobs, command* cmd, int pid);

/// @brief Update the status of a single background job
///
/// @param job Pointer to the job that will be changed
/// @param status The new job status. Retrieved by the waitpid function
void update_background_job_status(background_job* job, int status);

/// @brief Update the status of a single background job by it's process id
///
/// @param jobs The list of running jobs in the shell
/// @param pid The pid of the job that will be changed
/// @param status The new job status. Retrieved by the waitpid function
void update_background_job_status_by_pid(background_jobs* jobs, int pid, int status);

/// @brief Removes one job from the job list
///
/// @param jobs The list of running jobs in the shell
/// @param pid The pid of the job that will be removed
/// 
/// @return Returns the removed pid or -1 if the job doesn't exist
int remove_command_by_pid(background_jobs* jobs, int pid);

/// @brief Indicates if a single job is running or not
///
/// @param job Pointer to a job
///
/// @return Returns a boolean indicating if the job is running or not
bool job_running(background_job* job);

/// @brief Free only one background_job 
///
/// @param job Pointer to a job that will be freed
void free_background_job(background_job* job);

/// @brief Free the whole linked list of background jobs from an item
///
/// @param job Pointer to a job that will be freed
void free_background_job_chain(background_job* job);

/// @brief Free the whole linked list from the head
///
/// @param jobs The list of running jobs in the shell, that will be freed
void free_background_jobs(background_jobs* jobs);


#endif
