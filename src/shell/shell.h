#ifndef SHELL_HEADER
#define SHELL_HEADER


#include <stdbool.h>
#include <stdio.h>

#include "alias.h"
#include "command_parser.h"
#include "background_jobs.h"



typedef struct shell shell;
typedef struct _shell_builtin_commands shell_builtin_commands;


/** @brief Type definition of the prompt function
 *	@param shell The currently running shell, useful for some kind of prompts
 */
typedef void (*prompt_function)(shell* shell);


/** @brief Struct that define one shell **/
typedef struct shell {
	/**< Indicates if the shell is still running */
	bool running;

	/**< The function that will be executed to show the prompt */
	prompt_function prompt;

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

/// @brief Expand the command to match the given alias
///
/// @param cmd The command that will be expanded
/// @param alias The alias that will be expanded to
void expand_command_with_alias(command* cmd, alias alias);

/// @brief Run command from a single input string
///
///	@param shell The currently running shell
///	@param input The input line
void run_from_string(shell* shell, char* input);

/// @brief Run commands from the given file
///
///	@param shell The currently running shell
///	@param fp The input file
void run_from_file(shell* shell, FILE *fp);

/// @brief Run a single command chain
///
///	@param shell The running shell
/// @param cmd The command that will be executed
/// @param in_fd The file descriptor of input. Normally it will be STDIN_FILENO
void run_command(shell* shell, command* cmd, int in_fd);

/// @brief Run the shell interactively
///
/// @param The shell that will be executed
void run_interactive(shell* shell);

/// @brief Frees the given shell
/// @param shell The pointer that will be freed
void free_shell(shell* shell);


#endif
