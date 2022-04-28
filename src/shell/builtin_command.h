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



builtin_command* create_item(char* key, char* value);

shell_builtin_commands* create_shell_builtin_commands(int size);

unsigned long hash_function(shell_builtin_commands* commands, char* str);

void add_builtin_command(
	shell_builtin_commands* commands, 
	char* key, 
	BuiltinCommandFunction func
);

BuiltinCommandFunction find_builtin_command(shell_builtin_commands* commands, char* key);

void free_builtin_command(builtin_command* command);

void free_shell_builtin_commands(shell_builtin_commands* commands);


/**
 *   BUILTIN FUNCTIONS
 */

/** @todo Check naming style for builtin_functions */

int shell_cd(shell* shell, int argc, char** argv);

int shell_fg(shell* shell, int argc, char** argv);

int shell_jobs(shell* shell, int argc, char** argv);

int shell_time(shell* shell, int argc, char** argv);

int shell_fim(shell* shell, int argc, char** argv);
int shell_exit(shell* shell, int argc, char** argv);

int shell_help(shell* shell, int argc, char** argv);


#endif
