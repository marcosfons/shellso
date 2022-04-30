#include <stdlib.h>
#include <stdio.h>

#include "command.h"


command* command_create() {
	command* cmd = malloc(sizeof(command));
	if (cmd == NULL) {
		perror(NULL);
		exit(EXIT_FAILURE);
	}
	
	cmd->command = NULL;
	cmd->next = NULL;
	cmd->chain_type = NONE;
	cmd->argc = 0;
	cmd->argv = malloc(sizeof(char**));
	if (cmd->argv == NULL) {
		perror(NULL);
		exit(EXIT_FAILURE);
	}

	cmd->argv[0] = NULL;

	cmd->pid = PID_COMMAND_NOT_EXECUTED_YET;
	cmd->status = STATUS_COMMAND_NOT_EXECUTED_YET;

	return cmd;
}

void command_set_command(command* cmd, char* input) {
	cmd->command = input;

	command_add_argument(cmd, input);
}

void command_add_argument(command* cmd, char* argument) {
	cmd->argv = realloc(cmd->argv, (cmd->argc + 2) * sizeof(char*));
	if (cmd->argv == NULL) {
		perror(NULL);
		exit(EXIT_FAILURE);
	}

	cmd->argv[cmd->argc] = argument;
	cmd->argv[cmd->argc + 1] = NULL;
	cmd->argc += 1;
}

void command_set_stdout_redirection(command* cmd, char* filepath) {
	cmd->stdout_file_redirection = filepath;
}

void command_set_stdin_redirection(command* cmd, char* filepath) {
	cmd->stdin_file_redirection = filepath;
}

void command_free(command* command) {
	if (command == NULL) {
		return;
	} else if (command->next != NULL) {
		command_free(command->next);
	}

	free(command->command);

	// i = 1 because the first argument is the command that already were deallocated
	for (int i = 1; i < command->argc; i++) {
		free(command->argv[i]);
	}
	free(command->argv);
}
