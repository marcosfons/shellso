#include <assert.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <wait.h>


#include "alias.h"
#include "builtin_command.h"
#include "prompts.h"
#include "shell.h"
#include "background_jobs.h"
#include "command.h"
#include "command_parser.h"


#define READ_END 0
#define WRITE_END 1

#define ARBITRARY_ALIASSES_HASH_SIZE 43
#define ARBITRARY_BUILTIN_COMMANDS_HASH_SIZE 127


static shell* running_shell = NULL;

/// Handles Ctrl-C events, it does nothing.
/// exec already kill the child process
static void sigintHandler(int sig_num) {
	signal(SIGINT, sigintHandler); // Will receive the signal on the next time
	printf("\n");
}

static void sigchldHandler(int sig_num) {
	signal(SIGCHLD, sigchldHandler);

	background_job* curr = running_shell->jobs->next;
	while (curr != NULL) {
		int state;
		int res = waitpid(curr->pid, &state, WNOHANG);

		if (res > 0 && WIFEXITED(state)) {
			// printf("[+%d] Background process done. Status: %d\n", curr->pid, WEXITSTATUS(state));

			update_background_job_status(curr, state);
			printf("[+%d] Background updated. Status: %d\n", curr->pid, WEXITSTATUS(state));
			break;
		}

		curr = curr->next;
	}
}

shell* create_shell() {
	if (running_shell != NULL && running_shell->running) {	
		printf("Only one shell can run at the same time. Stop the running shell\n");
		return NULL;
	}

	running_shell = malloc(sizeof(shell));

	running_shell->running = true;
	running_shell->prompt = &normal_shell_prompt;
	running_shell->aliasses = create_aliasses(ARBITRARY_ALIASSES_HASH_SIZE);
	running_shell->jobs = create_background_jobs(NULL);
	running_shell->builtin_commands = create_shell_builtin_commands(ARBITRARY_BUILTIN_COMMANDS_HASH_SIZE);

	add_builtin_command(running_shell->builtin_commands, "cd", &shell_cd);
	// add_builtin_command(running_shell->builtin_commands, "fg", &shell_fg);
	// add_builtin_command(running_shell->builtin_commands, "help", &shell_help);
	add_builtin_command(running_shell->builtin_commands, "jobs", &shell_jobs);
	add_builtin_command(running_shell->builtin_commands, "exit", &shell_exit);
	add_builtin_command(running_shell->builtin_commands, "fim", &shell_exit);
	// add_builtin_command(running_shell->builtin_commands, "time", &shell_time);
	add_builtin_command(running_shell->builtin_commands, "alias", &shell_alias);
	
	char** ls = malloc(2 * sizeof(char*));
	ls[0] = strdup("ls");
	ls[1] = strdup("--color=auto");

	char** grep = malloc(2 * sizeof(char*));
	grep[0] = strdup("grep");
	grep[1] = strdup("--color=auto");
	add_alias(running_shell->aliasses, ls[0], 2, ls);
	add_alias(running_shell->aliasses, grep[0], 2, grep);

	return running_shell;
}

void print_command_error(char* command, char* error) {
	if (error == NULL) {
		fprintf(stderr, "shellso: %s: An error has occurred\n", command);
	} else {
		fprintf(stderr, "shellso: %s: %s\n", command, error);
	}
}

static char* read_input() {
	char* input = NULL;
	size_t length = 0;

	int nread = getline(&input, &length, stdin);
	if (nread == -1) {
		free(input);
		return NULL;
	}

	if (input[nread - 1] == '\n') { // Replace newline
		input[nread - 1] = '\0';
	}

	return input;
}


/// @todo Check if this code must be in shell, alias or command
/// Maybe it's better in command, because it's changing it
void expand_command_with_alias(command* cmd, alias alias) {
	// Minus 1 because in both there is a command cmd[0] and alias[0]
	int new_size = cmd->argc + alias.argc;
	cmd->argv = realloc(cmd->argv, new_size * sizeof(char*));

	for (int i = 1; i < cmd->argc; i++) {
		cmd->argv[alias.argc - 1 + i] = cmd->argv[i];
	}

	for (int i = 1; i < alias.argc; i++) {
		cmd->argv[i] = strdup(alias.argv[i]);
	}

	cmd->argv[new_size - 1] = NULL;

	cmd->argc = new_size;
}

void run_from_file(shell* shell, FILE* fp) {

}

void run_from_string(shell* shell, char* input) {
	command* cmd = command_parse(input);

	if (cmd != NULL) {
		// Problem with this approach is that it'll only work if the first program is a builtin
		builtin_command_function func = find_builtin_command(shell->builtin_commands, cmd->argv[0]);
		if (func != NULL) {
			int status = func(shell, cmd->argc, cmd->argv);
		} else {
			// add_command_chain(interactive_shell->jobs, cmd);
			// add_command_chain(jobs, cmd);
			run_command(shell, cmd, STDIN_FILENO);
			dup(STDIN_FILENO);
			dup(STDOUT_FILENO);
		}

		command_free(cmd);
	}
}

void run_interactive(shell* shell) {
	signal(SIGINT, sigintHandler);
	signal(SIGCHLD, sigchldHandler);

	do {
		running_shell->prompt(running_shell);
		char* input = read_input();

		run_from_string(shell, input);

		if (input != NULL) {
			free(input);
		}		
	} while (running_shell->running && !feof(stdin));
}

static void CLOSE(int fd) {
	int res = close(fd);
	if (res == -1) {
		perror("CLOSE: ");
	}
}

static void redirect_and_close(int oldfd, int newfd) {
  if (oldfd != newfd) {
    if (dup2(oldfd, newfd) != -1)
      CLOSE(oldfd); /* successfully redirected */
  }
}

void run_command(shell* shell, command* cmd, int in_fd) {
	int fd[2] = {-1, -1};

	alias* alias = find_alias(shell->aliasses, cmd->argv[0]);
	if (alias != NULL) {
		expand_command_with_alias(cmd, *alias);
	}

	if (cmd->chain_type == PIPE) {
		if (pipe(fd) == -1) {
			perror("pipe:");
		}
	}

	pid_t child_pid = fork();	

	if (child_pid == -1) {
		perror("Fork");
	} else if(child_pid == 0) { // Child code
	
		if (cmd->chain_type == PIPE) {
			CLOSE(fd[READ_END]);

			if (cmd->stdin_file_redirection != NULL) {
				FILE* stdin_file = fopen(cmd->stdin_file_redirection, "r");
				redirect_and_close(stdin_file->_fileno, STDIN_FILENO);
			} else {
				redirect_and_close(in_fd, STDIN_FILENO);
			}

			if (cmd->stdout_file_redirection != NULL) {
				FILE* stdout_file = fopen(cmd->stdout_file_redirection, "w");
				redirect_and_close(stdout_file->_fileno, STDOUT_FILENO);
			} else {
				redirect_and_close(fd[WRITE_END], STDOUT_FILENO);
			}
		} else {
			// This is important because of the last command of a pipe command chain
			redirect_and_close(in_fd, STDIN_FILENO);
		}

		execvp(cmd->argv[0], cmd->argv);
		print_command_error(cmd->argv[0], strerror(errno));
		
		exit(EXIT_FAILURE); // Exits only the child process
	} else { // Parent code
		add_background_job(shell->jobs, cmd, child_pid);

		if (cmd->chain_type == BACKGROUND) {
			// add_command_chain(background_jobs *jobs, command *command_chain)
			if (cmd->next != NULL) {
				run_command(shell, cmd->next, in_fd);
			}
		} else if (cmd->chain_type == PIPE) {
			CLOSE(fd[WRITE_END]);

			run_command(shell, cmd->next, fd[0]);
		} else {
			int state;
			int result = waitpid(child_pid, &state, 0);
			if (result == -1 && errno != 10) { 
				perror("waitpid");
				exit(EXIT_FAILURE);
			}

			if (cmd->next != NULL) {
				if ((cmd->chain_type == AND && WEXITSTATUS(state) == EXIT_SUCCESS) || 
						(cmd->chain_type == OR)) {
					run_command(shell, cmd->next, in_fd);
				}
			}
		}
	}
}

void free_shell(shell *shell) {
	if (shell == NULL) {
		return;
	}
	
	// Just for completeness
	shell->running = false;

	free_shell_builtin_commands(shell->builtin_commands);
	free_background_jobs(shell->jobs);
	free_aliasses(shell->aliasses);

	free(shell);
}

