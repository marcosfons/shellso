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
static void sigint_handler(int sig_num) {
	signal(SIGINT, sigint_handler); // Will receive the signal on the next time
	printf("\n");
	running_shell->prompt(running_shell);
	// Flush because the prompt usually does not have a newline in the end
	fflush(stdout);
}

static void sigstop_handler(int sig_num) {
	signal(SIGTSTP, sigstop_handler);
	printf("SIGSTOP PROCESS\n");
}

static void sigchld_handler(int sig_num) {
	signal(SIGCHLD, sigchld_handler);

	background_job* curr = running_shell->jobs->next;
	while (curr != NULL) {
		int state;
		int res = waitpid(curr->pid, &state, WNOHANG);

		if (res > 0 && WIFEXITED(state)) {
			update_background_job_status(curr, state);
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
	running_shell->verbose = false;
	running_shell->prompt = &normal_shell_prompt;
	running_shell->aliasses = create_aliasses(ARBITRARY_ALIASSES_HASH_SIZE);
	running_shell->jobs = create_background_jobs(NULL);
	running_shell->builtin_commands = create_shell_builtin_commands(ARBITRARY_BUILTIN_COMMANDS_HASH_SIZE);

	add_builtin_command(running_shell->builtin_commands, "cd", &shell_cd);
	add_builtin_command(running_shell->builtin_commands, "fg", &shell_fg);
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

static char* read_input_from_file(FILE* file) {
	char* input = NULL;
	size_t length = 0;

	int nread = getline(&input, &length, file);
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

	char** new_argv = malloc(new_size * sizeof(char*));

	for (int i = 1; i < cmd->argc; i++) {
		new_argv[alias.argc - 1 + i] = cmd->argv[i];
	}

	for (int i = 0; i < alias.argc; i++) {
		new_argv[i] = strdup(alias.argv[i]);
	}

	new_argv[new_size - 1] = NULL;

	free(cmd->argv[0]);
	free(cmd->argv);
	cmd->argc = new_size;

	cmd->argv = new_argv;
}

void run_from_string(shell* shell, char* input) {
	command* cmd = command_parse(input);
	if (cmd == NULL) {
		return;
	}

	if (shell->verbose) {
		printf("%s\n", input);
	}

	run_command(shell, cmd, STDIN_FILENO);
	// Unnecessary
	// dup(STDIN_FILENO);
	// dup(STDOUT_FILENO);

	command_free(cmd);
}

void run_from_file(shell* shell, FILE* file) {
	signal(SIGINT, sigint_handler);
	signal(SIGCHLD, sigchld_handler);
	signal(SIGTSTP, sigstop_handler);

	do {
		if (file == stdin) {
			running_shell->prompt(running_shell);
		}
		char* input = read_input_from_file(file);

		run_from_string(shell, input);

		if (input != NULL) {
			free(input);
		}		
	} while (running_shell->running && !feof(file));
}

void run_interactive(shell* shell) {
	run_from_file(shell, stdin);
}

static void close_fd(int fd) {
	int res = close(fd);
	if (res == -1) {
		perror("close");
	}
}

static void redirect_and_close(int oldfd, int newfd) {
  if (oldfd != newfd) {
    if (dup2(oldfd, newfd) != -1) {
      close_fd(oldfd); /* successfully redirected */
		}
  }
}

static void redirect_to_file(char* filepath, char* mode, int redirect_fd) {
	FILE* file = fopen(filepath, mode);
	if (file == NULL) {
		perror("fopen");
		exit(EXIT_FAILURE); // Exits only the child process
	}

	redirect_and_close(file->_fileno, redirect_fd);
}

// This function is only executed by the child process
static void execute_command(command* cmd, int fd[2], int in_fd) {
	if (cmd->stdin_file_redirection != NULL) {
		redirect_to_file(cmd->stdin_file_redirection, "r", in_fd);
	}	
	if (cmd->stdout_file_redirection != NULL) {
		redirect_to_file(cmd->stdout_file_redirection, "w+", STDOUT_FILENO);
	}

	if (cmd->chain_type == PIPE) {
		close_fd(fd[READ_END]);

		if (cmd->stdin_file_redirection == NULL) {
			redirect_and_close(in_fd, STDIN_FILENO);
		}
		if (cmd->stdout_file_redirection == NULL) {
			redirect_and_close(fd[WRITE_END], STDOUT_FILENO);
		}
	} else {
		// This is important because of the last command of a pipe command chain
		redirect_and_close(in_fd, STDIN_FILENO);
	}

	execvp(cmd->argv[0], cmd->argv);
	print_command_error(cmd->argv[0], strerror(errno));
	
	exit(EXIT_FAILURE); // Exits only the child process
}


void run_command(shell* shell, command* cmd, int in_fd) {
	// Try to run a builtin command first
	builtin_command_function func = find_builtin_command(shell->builtin_commands, cmd->argv[0]);
	if (func != NULL) {
		int status = func(shell, cmd->argc, cmd->argv);

		if (cmd->next != NULL) {
			if (cmd->chain_type == AND && status == EXIT_SUCCESS) {
				run_command(shell, cmd->next, in_fd);
			} else if (cmd->chain_type == OR && status != EXIT_SUCCESS) {
				printf("OR\n");
				run_command(shell, cmd->next, in_fd);
			} else if (cmd->chain_type != AND && cmd->chain_type != OR) {
				run_command(shell, cmd->next, in_fd);
			}
		}

		return;
	}

	alias* alias = find_alias(shell->aliasses, cmd->argv[0]);
	if (alias != NULL) {
		expand_command_with_alias(cmd, *alias);
	}

	int fd[2];
	if (cmd->chain_type == PIPE) {
		if (pipe(fd) == -1) {
			perror("pipe:");
		}
	}

	pid_t child_pid = fork();	

	if (child_pid == -1) {
		perror("Fork");
	} else if(child_pid == 0) { // Child code
		execute_command(cmd, fd, in_fd);
	} else { // Parent code
		add_background_job(shell->jobs, cmd, child_pid);

		if (cmd->chain_type == BACKGROUND) {
			if (cmd->next != NULL) {
				run_command(shell, cmd->next, in_fd);
			}
		} else if (cmd->chain_type == PIPE) {
			close_fd(fd[WRITE_END]);

			run_command(shell, cmd->next, fd[0]);
		} else {
			int state;
			int result = waitpid(child_pid, &state, WUNTRACED);
			if (result == -1 && errno != 10) { 
				perror("waitpid");
			}
			
			update_background_job_status_by_pid(shell->jobs, child_pid, state);

			if (cmd->next != NULL) {
				if ((cmd->chain_type == AND && WEXITSTATUS(state) == EXIT_SUCCESS) || 
						(cmd->chain_type == OR  && WEXITSTATUS(state) != EXIT_SUCCESS)) {
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

