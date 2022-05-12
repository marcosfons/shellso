#ifndef SHELL_HEADER
#define SHELL_HEADER


#include <stdio.h>

#include "alias.h"
#include "command_parser.h"
#include "background_jobs.h"


/** @brief Struct that define one shell **/
typedef struct _shell_builtin_commands shell_builtin_commands;
typedef struct {
	/**< All the aliasses of the shell */
	aliasses* aliasses;

	/**< The background jobs of the shell */
	background_jobs* jobs;

	/**< The builtin commands of the shell */
	shell_builtin_commands* builtin_commands;
} shell;


/// @brief Create a shell
///
/// Only one shell can be running at the same time. It's
/// unsafe to run more than one.
/// Problems: Signal handling, reallocation of memory
///
/// @return Pointer to newly created shell
shell* create_shell();

void print_command_error(char* command, char* error);

/// @brief Run a single command chain
///
/// @param cmd The command that will be executed
/// @param in_fd The file descriptor of input. Normally it will be STDIN_FILENO
void run_command(command* cmd, int in_fd);

/// @brief Run the shell interactively
///
/// @param The shell that will be executed
void run_interactive(shell* shell);

void run_for_file(FILE *fp);

void run_for_line(char* input);

/// @brief Frees the given shell
/// @param shell The pointer that will be freed
void free_shell(shell* shell);


#endif
