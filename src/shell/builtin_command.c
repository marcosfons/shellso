#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>


#include "builtin_command.h"


static builtin_command* create_builtin_command(char* command, builtin_command_function func) {
	builtin_command* item = malloc(sizeof(builtin_command));

	item->command = command;
	item->function = func;

	return item;
}

shell_builtin_commands* create_shell_builtin_commands(int size) {
	shell_builtin_commands* commands = malloc(sizeof(shell_builtin_commands));
	
	commands->size = size;
	commands->count = 0;

	// commands->commands = calloc(commands->size, sizeof(builtin_command));
	commands->commands = calloc(commands->size, sizeof(builtin_command*));

	for (int i = 0; i < commands->size; i++) {
		commands->commands[i] = NULL;
	}

	return commands;
}

unsigned long builtin_command_hash_function(shell_builtin_commands* commands, char* str) {
	unsigned long i = 0;
	for (int j=0; str[j]; j++) {
		i += str[j];
	}
	return i % commands->size;
}

void add_builtin_command(
	shell_builtin_commands* commands, 
	char* key, 
	builtin_command_function func
) {
	builtin_command* item = create_builtin_command(key, func);

	unsigned long index = builtin_command_hash_function(commands, key);

	builtin_command* current_item = commands->commands[index];
	 
	if (current_item == NULL) {
		if (commands->count == commands->size) {
			fprintf(stderr, "Builtin commands are fullfilled\n");
			free_builtin_command(item);
			return;
		}
		 
		commands->commands[index] = item; 
		commands->count++;
	} else {
		// Scenario 1: We only need to update value
		if (strcmp(current_item->command, key) == 0) {
			commands->commands[index]->function = func;
			return;
		} else {
			fprintf(stderr, "Collision in item %s\n", key);
			// Scenario 2: Collision
			// We will handle case this a bit later
			// handle_collision(commands, index, item);
			return;
		}
	}
}

builtin_command_function find_builtin_command(shell_builtin_commands* commands, char* key) {
	if (key == NULL || key[0] == '\0') {
		return NULL;
	}

	int index = builtin_command_hash_function(commands, key);
	builtin_command* item = commands->commands[index];

	// Ensure that we move to a non NULL item
	if (item != NULL) {
		if (strcmp(item->command, key) == 0) {
			return item->function;
		}
	}
	return NULL;
}

void free_builtin_command(builtin_command* command) {
	// Commenting this line because most of the time it will be stack strings
	// add_builtin_command(builtin, "SOME_STRING", some_function)
	// So this will throw munmap_chunk(): invalid pointer
	// free(command->command);
	free(command);
}
 
void free_shell_builtin_commands(shell_builtin_commands* commands) {
	for (int i = 0; i < commands->size; i++) {
		builtin_command* item = commands->commands[i];
		if (item != NULL) {
			free_builtin_command(item);
		}
	}

	free(commands->commands);
	free(commands);
}


/**
	* BUILTIN FUNCTIONS
	*/
int shell_cd(shell* shell, int argc, char** argv) {
	if (argc > 2) {
		print_command_error("cd", "Too many arguments");
		return EXIT_FAILURE;
	}

	if (chdir(argc == 2 ? argv[1] : getenv("HOME")) == -1) {
		print_command_error("cd", strerror(errno));
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

int shell_exit(shell* shell, int argc, char** argv) {
	shell->running = false;

	return EXIT_SUCCESS;
}

int shell_alias(shell* shell, int argc, char** argv) {
	for (int i = 0; i < shell->aliasses->size; i++) {
		alias* alias = shell->aliasses->aliasses[i];
		if (alias != NULL) {
			printf("alias %s='", alias->key);
			for (int j = 0; j < alias->argc; j++) {
				printf("%s ", alias->argv[j]);
			}
			printf("\b'\n");
			
		}
	}

	return EXIT_SUCCESS;
}

int shell_jobs(shell* shell, int argc, char** argv) {
	background_job* curr = shell->jobs->next;
	while (curr != NULL) {
		const int BUFFER_STATE_SIZE = 37;
		char state[BUFFER_STATE_SIZE];
		// printf("Status: %s %d\n", curr->command, curr->status);
		if (curr->status == STATUS_COMMAND_NOT_EXECUTED_YET) {
			strcpy(state, "Running");
		} else {
			// Based on https://mesos.apache.org/api/latest/c++/status__utils_8hpp_source.html
			if (WIFEXITED(curr->status)) {
				snprintf(state, BUFFER_STATE_SIZE, "Exited with status %d", WEXITSTATUS(curr->status));
			} else if (WIFSIGNALED(curr->status)) {
				strncpy(state, strsignal(WTERMSIG(curr->status)), BUFFER_STATE_SIZE);
			} else if (WIFSTOPPED(curr->status)) {
				strncpy(state, strsignal(WSTOPSIG(curr->status)), BUFFER_STATE_SIZE);
			} else {
				snprintf(state, BUFFER_STATE_SIZE, "Waiting status %d", curr->status);
			}

			// @todo NEED TO REMOVE COMMANDS
			// If remove inside the loop it will break
			// remove_command_by_pid(shell->jobs, curr->pid);
		}

		printf("[%d] %-35s  %s\n", curr->pid, state, curr->command);

		curr = curr->next;
	}
	
	return 0;
}

int shell_fg(shell* shell, int argc, char** argv) {
	if (kill(shell->jobs->next->pid, SIGCONT) != 0) {
		printf("Some kill error\n");
		return 1;
	}

	int state;
	int result = waitpid(shell->jobs->next->pid, &state, 0);
	printf("CONSEGUIU SAIR KRAI  %d %d\n", state, result);
	if (result == -1 && errno != 10) { 
		perror("waitpid");
		exit(EXIT_FAILURE);
	}
	update_background_job_status_by_pid(shell->jobs, shell->jobs->next->pid, state);



	return 0;
}
//
// int shell_time(shell* shell, int argc, char** argv);

//
// int shell_help(shell* shell, int argc, char** argv);

