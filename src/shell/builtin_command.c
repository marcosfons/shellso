#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "builtin_command.h"


static builtin_command* create_builtin_command(char* command, BuiltinCommandFunction func) {
	builtin_command* item = malloc(sizeof(builtin_command));

	item->command = command;
	item->function = func;

	return item;
}

shell_builtin_commands* create_shell_builtin_commands(int size) {
	shell_builtin_commands* commands = malloc(sizeof(shell_builtin_commands));
	
	commands->size = size;
	commands->count = 0;
	commands->commands = calloc(commands->size, sizeof(builtin_command));

	for (int i = 0; i < commands->size; i++) {
		commands->commands[i] = NULL;
	}

	return commands;
}

unsigned long hash_function(shell_builtin_commands* commands, char* str) {
	unsigned long i = 0;
	for (int j=0; str[j]; j++) {
		i += str[j];
	}
	return i % commands->size;
}

void add_builtin_command(
	shell_builtin_commands* commands, 
	char* key, 
	BuiltinCommandFunction func
) {
	builtin_command* item = create_builtin_command(key, func);

	unsigned long index = hash_function(commands, key);

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

BuiltinCommandFunction find_builtin_command(shell_builtin_commands* commands, char* key) {
	int index = hash_function(commands, key);
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
