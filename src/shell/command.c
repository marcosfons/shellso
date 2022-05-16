#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "command.h"


command* command_create() {
	command* cmd = malloc(sizeof(command));
	if (cmd == NULL) {
		perror(NULL);
		exit(EXIT_FAILURE);
	}
	
	// cmd->command = NULL;
	cmd->next = NULL;
	cmd->chain_type = NONE;
	cmd->argc = 0;
	cmd->argv = malloc(sizeof(char**));
	if (cmd->argv == NULL) {
		perror(NULL);
		exit(EXIT_FAILURE);
	}

	cmd->argv[0] = NULL;

	cmd->stdin_file_redirection = NULL;
	cmd->stdout_file_redirection = NULL;
	cmd->stderr_file_redirection = NULL;

	return cmd;
}

void command_set_command(command* cmd, char* input) {
	assert(cmd->argc == 0);

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

	for (int i = 0; i < command->argc; i++) {
		free(command->argv[i]);
	}
	free(command->argv);
	
	if (command->stdin_file_redirection != NULL)
		free(command->stdin_file_redirection);

	if (command->stdout_file_redirection != NULL)
		free(command->stdout_file_redirection);

	if (command->stderr_file_redirection != NULL)
		free(command->stderr_file_redirection);

	free(command);
}
