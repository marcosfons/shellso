#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include <errno.h>
#include <signal.h>
#include <assert.h>
#include <limits.h>


#include "builtin_command.h"
#include "shell.h"
#include "background_jobs.h"
#include "command.h"
#include "command_parser.h"


#define READ_END 0
#define WRITE_END 1

#define ARBITRARY_BUILTIN_COMMANDS_HASH_SIZE 103


/// Handles Ctrl-C events, it does nothing.
/// exec already kill the child process
static void sigintHandler(int sig_num) {
	signal(SIGINT, sigintHandler); // Will receive the signal on the next time
	printf("\n");
}

static void sigchldHandler(int sig_num) {
	signal(SIGCHLD, sigchldHandler);
	//
	// command* curr = jobs->next;
	// while (curr != NULL) {
	// 	curr = curr->next;
	//
	// 	int state;
	// 	int res = waitpid(curr->pid, &state, WNOHANG);
	//
	// 	if (res > 0 && WIFEXITED(state)) {
	// 		// printf("[+%d] Background process done. Status: %d\n", curr->pid, WEXITSTATUS(state));
	//
	// 		update_command_by_pid(jobs, curr->pid, state);
	// 		printf("[+%d] Background updated. Status: %d\n", curr->pid, WEXITSTATUS(state));
	// 		break;
	// 	}	
	// }
}

static void print_prompt() {
	size_t length = PATH_MAX;
	char* cwd = malloc(length);

	if (getcwd(cwd, length) == NULL) {
		perror("cwd");
		printf("%s$ ", "marcos");
	} else {
		printf("%s:%s$ ", "marcos", cwd);
	}
}

shell* create_shell() {
	shell* new_shell = malloc(sizeof(shell));

	new_shell->jobs = create_background_jobs(NULL);
	new_shell->builtin_commands = create_shell_builtin_commands(ARBITRARY_BUILTIN_COMMANDS_HASH_SIZE);

	add_builtin_command(new_shell->builtin_commands, "cd", &shell_cd);
	// add_builtin_command(new_shell->builtin_commands, "fg", &shell_fg);
	// add_builtin_command(new_shell->builtin_commands, "help", &shell_help);
	// add_builtin_command(new_shell->builtin_commands, "jobs", &shell_jobs);
	// add_builtin_command(new_shell->builtin_commands, "exit", &shell_exit);
	// add_builtin_command(new_shell->builtin_commands, "fim", &shell_fim);
	// add_builtin_command(new_shell->builtin_commands, "time", &shell_time);

	return new_shell;
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

	print_prompt();
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

void run_interactive() {
	shell* interactive_shell = create_shell();

	signal(SIGINT, sigintHandler);
	signal(SIGCHLD, sigchldHandler);

	command* cmd = NULL;

	do {
		char* input = read_input();

		cmd = command_parse(input);

		if (input != NULL) {
			free(input);
		}
		
		BuiltinCommandFunction func = find_builtin_command(interactive_shell->builtin_commands, cmd->command);
		if (func != NULL) {
			func(interactive_shell, cmd->argc, cmd->argv);
		} else {

			if (cmd != NULL) {
				// add_command_chain(interactive_shell->jobs, cmd);
				// add_command_chain(jobs, cmd);
				run_command(cmd, STDIN_FILENO);
			}
			dup(STDIN_FILENO);
			dup(STDOUT_FILENO);

			// dup2(STDIN_FILENO, STDIN_FILENO);
			// dup2(STDOUT_FILENO, STDOUT_FILENO);
			// close(STDIN_FILENO);
			// close(STDOUT_FILENO);
			// fflush(stdout);
		}
		usleep(5000); // Arbitrary sleep
	} while (!feof(stdin) && (cmd == NULL || strcmp(cmd->command, "quit") != 0));
	
	if (cmd != NULL) {
		command_free(cmd);
	}
}

static void redirect_and_close(int oldfd, int newfd) {
  if (oldfd != newfd) {
    if (dup2(oldfd, newfd) != -1)
      close(oldfd); /* successfully redirected */
  }
}

static void CLOSE(int fd) {
	int res = close(fd);
	if (res == -1) {
		perror("CLOSE: ");
	}
}

void run_for_line(char* input) {

	command* cmd = command_parse(input);
	if (input != NULL) {
		free(input);
	}

	if (cmd != NULL) {
		run_command(cmd, STDIN_FILENO);
	}
}

void run_command(command* cmd, int in_fd) {
	int fd[2] = {-1, -1};

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
			// printf("Piping\n");

			CLOSE(fd[READ_END]);

			if (cmd->stdin_file_redirection != NULL) {
				FILE* stdin_file = fopen(cmd->stdin_file_redirection, "r");
				redirect_and_close(in_fd, stdin_file->_fileno);
			} else {
				redirect_and_close(in_fd, STDIN_FILENO);
			}

			if (cmd->stdout_file_redirection != NULL) {
				FILE* stdout_file = fopen(cmd->stdout_file_redirection, "w");
				redirect_and_close(in_fd, stdout_file->_fileno);
			} else {
				redirect_and_close(fd[WRITE_END], STDOUT_FILENO);
			}
			// CLOSE(in_fd);
			// CLOSE(fd[READ_END]);
		} else {
			redirect_and_close(in_fd, STDIN_FILENO);
			// redirect_and_close(1, STDOUT_FILENO);
		}
		// } else {
		// 	// CLOSE(fd[0]);
		// 	// CLOSE(fd[1]);
		// 	redirect_and_close(in_fd, STDIN_FILENO);
		// 	// CLOSE(in_fd);
		// 	// CLOSE(fd[WRITE_END]);
		// 	// CLOSE(fd[READ_END]);
		// }

    execvp(cmd->command, cmd->argv);
		print_command_error(cmd->command, strerror(errno));
		
		exit(EXIT_FAILURE); // Exits only the child process
	} else { // Parent code
		cmd->pid = child_pid;

		if (cmd->chain_type == BACKGROUND) {
			if (cmd->next != NULL) {
				run_command(cmd->next, in_fd);
			}
		} else if (cmd->chain_type == PIPE) {
			// dup2(fd[READ_END], in_fd);
			CLOSE(fd[WRITE_END]);
			CLOSE(in_fd);
			if (cmd->next != NULL) {
				run_command(cmd->next, fd[0]);
			}
		} else {
			int state;
			int result = waitpid(child_pid, &state, 0);
			if (result == -1) { 
				perror("waitpid");
				exit(EXIT_FAILURE);
			}

			if (cmd->next != NULL) {
				if ((cmd->chain_type == AND && WEXITSTATUS(state) == 0) || 
						(cmd->chain_type == OR)) {
					run_command(cmd->next, in_fd);
				}
			}
		}
	}
}


