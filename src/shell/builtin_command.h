#ifndef BUILTIN_COMMAND_HEADER
#define BUILTIN_COMMAND_HEADER


#include <stdio.h>

#include "shell.h"

/** @todo Add documentation to each function */

/** @brief Type of a builtin command function that will be executed */
typedef int (*BuiltinCommandFunction)(shell* shell, int argc, char** argv);

/** @brief Struct to represent a shell builtin command */
typedef struct {
	char* command;
	BuiltinCommandFunction function;
} builtin_command;

/** @brief HashTable containing every shell builtin command */
typedef struct _shell_builtin_commands {
    builtin_command** commands;
    int size;
    int count;
} shell_builtin_commands;


/** @brief Create the hash map of builtin commands
 *	
 *	@param size The initial size of the hash map
 *
 *	@return a pointer to the newly created builtin commands hash map
 */
shell_builtin_commands* create_shell_builtin_commands(int size);

unsigned long hash_function(shell_builtin_commands* commands, char* str);

/** @brief Adds one builtin command using the key to hash it
 *	
 *	@todo Add documentation of commands in add_builtin_command
 *	@param commands
 *	@param key The key param that will be used in the hash
 *	@param func The function that will be executed for this builtin command
 */
void add_builtin_command(
	shell_builtin_commands* commands, 
	char* key, 
	BuiltinCommandFunction func
);

/** @brief Find one builtin command by searching with one key
 *	
 *	@todo Add documentation of commands in add_builtin_command
 *	@param commands
 *	@param key The key param that will be used in the hash
 *
 *	@return the function that must be executed for this builtin command
 */
BuiltinCommandFunction find_builtin_command(shell_builtin_commands* commands, char* key);

/** @brief Frees the given builtin_commands
 *	
 *	@param command The builtin command that will be freed
 */
void free_builtin_command(builtin_command* command);

/** @brief Frees the given builtin_commands
 *	
 *	@param commands The builtin commands that will be freed
 */
void free_shell_builtin_commands(shell_builtin_commands* commands);


/**
 *   BUILTIN FUNCTIONS
 */

/** @todo Check naming style for builtin_functions */

/** @brief Change directory builtin function
 *	
 *	@param shell The shell where this command it's being executed
 *	@param argc The argument count
 *	@param argv The argument values
 *
 *	@return Return an integer according to the end status
 */
int shell_cd(shell* shell, int argc, char** argv);

/** @brief Try to bring one background process to foreground
 *	
 *	@param shell The shell where this command it's being executed
 *	@param argc The argument count
 *	@param argv The argument values
 *
 *	@return Return an integer according to the end status
 */
int shell_fg(shell* shell, int argc, char** argv);

/** @brief List all background jobs running in the shell
 *	
 *	@param shell The shell where this command it's being executed
 *	@param argc The argument count
 *	@param argv The argument values
 *
 *	@return Return an integer according to the end status
 */
int shell_jobs(shell* shell, int argc, char** argv);

/** @brief Give time measures for one command
 *	
 *	@param shell The shell where this command it's being executed
 *	@param argc The argument count
 *	@param argv The argument values
 *
 *	@return Return an integer according to the end status
 */
int shell_time(shell* shell, int argc, char** argv);

/** @brief Finishes the shell
 *	
 *	@param shell The shell where this command it's being executed
 *	@param argc The argument count
 *	@param argv The argument values
 *
 *	@return Return an integer according to the end status
 */
int shell_fim(shell* shell, int argc, char** argv);

/** @brief Exit from the shell
 *	
 *	@param shell The shell where this command it's being executed
 *	@param argc The argument count
 *	@param argv The argument values
 *
 *	@return Return an integer according to the end status
 */
int shell_exit(shell* shell, int argc, char** argv);

/** @brief Show a help message to use the shell
 *	
 *	@param shell The shell where this command it's being executed
 *	@param argc The argument count
 *	@param argv The argument values
 *
 *	@return Return an integer according to the end status
 */
int shell_help(shell* shell, int argc, char** argv);


#endif
