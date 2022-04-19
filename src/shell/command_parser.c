#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "command_parser.h"


typedef enum parser_state {
	WAITING_FIRST_COMMAND_CHARACTER,
	READING_COMMAND,
	WAITING_FIRST_ARGUMENT_CHARACTER,
	READING_ARGUMENTS
} parser_state;


/* Reallocate dest string considering the escaped character
 *
 * input[escaping_character_position] must be the escaping character
 * It will change both dest and size string
 * 
 * Arguments:
 *	char* dest											The string that will be changed
 *	int* size												The dest string size, considering null-terminator
 *	char* input
 *	int escaping_character_position The position of the escaping character in the input string
 *
 */
static char* str_realloc_escaping_character(char* dest, int* size, char* input, int escaping_character_position) {
	dest = realloc(dest, (*size + escaping_character_position + 1) * sizeof(char));

	strncpy(dest + (*size - 1), input, escaping_character_position + 1);

	dest[*size + escaping_character_position] = '\0';
	dest[*size + escaping_character_position - 1] = input[escaping_character_position + 1];

	*size = *size + escaping_character_position + 1;
	
	return dest;
}


/* Extract the content between quotes inside input into dest and change the size
 *
 * It will reallocate dest to add space for the content between quotes
 * dest must be an allocated pointer
 * For double quotes it handles escaping characters
 *
 * Arguments:
 * 	char* dest
 * 	int size 		The size of the dest string before concatenating. dest[size - 1] == '\0'
 *	char* input The input string, the initial character must be " or '
 *
 * Returns a int indicating the position of the last quote, if the 
 * last quote was not found it returns -1
 *
 */
static int read_between_quotes(char* dest, int* size, char* input) {
	assert(dest[*size - 1] == '\0');
	char quote_type = input[0]; // Will be " or '
	assert(quote_type == '"' || quote_type == '\'');
	input += 1; // Shift
	int i = 0;
	int size_c = *size;

	if (quote_type == '\'') {
		for (; input[i] != quote_type && input[i] != '\0'; i++) {}
		if (input[i] != '\0') {
			dest = realloc(dest, (size_c + i) * sizeof(char));
			strncpy(dest + size_c - 1, input, i);
			dest[size_c + i - 1] = '\0';
			size_c += i;
		}
	} else {
		int j = 0;
		for (; input[j] != quote_type && input[j] != '\0'; j++) {
			if (input[j] == '\\') {
				str_realloc_escaping_character(dest, &size_c, input, j);
				input += j + 2;
				i += j + 2;
				j = -1;
			}
		}
		i += j;
		dest = realloc(dest, (size_c + j) * sizeof(char));
		strncpy(dest + size_c - 1, input, j);
		size_c += j;
		dest[size_c + j - 1] = '\0';
	}
	// Reached the end of the string without finding the end quote
	if (input[i] == '\0') {
		fprintf(stderr, "Unexpected EOF while looking for matching %c\n", quote_type);
		return -1;
	}
	*size = size_c;

	return i + 1;
}


static char* allocate_empty_string() {
	char* string = malloc(sizeof(char));
	string[0] = '\0';
	return string;
}

command_t* command_create() {
	command_t* command = malloc(sizeof(command_t));
	
	command->command = NULL;
	command->argc = 0;
	command->argv = malloc(sizeof(char**));
	command->chain_type = NONE;
	command->next = NULL;

	return command;
}

void command_set_command(command_t* command, char* input) {
	command->command = input;
}

void command_add_argument(command_t* command, char* argument) {
	command->argv = realloc(command->argv, (command->argc + 1) * sizeof(char*));
	command->argv[command->argc] = argument;
	command->argc += 1;
}

command_t* command_parse(char* input) {
	if (input == NULL) {
		return NULL;
	}

	command_t* command = command_create();

	parser_state state = WAITING_FIRST_COMMAND_CHARACTER;

	const char* restricted_chars = "&| \0\\=><";
	
	char* content = allocate_empty_string();
	int content_size = 1;

	int i = -1;

	do {
		i++;

		// Restricted chars
		if (strchr(restricted_chars, input[i]) != NONE) {
			if (state == READING_COMMAND || state == READING_ARGUMENTS) {
				content = realloc(content, (content_size + i) * sizeof(char));
				strncpy(content + content_size - 1, input, i);

				if (state == READING_COMMAND) {
					command_set_command(command, content);
					state = WAITING_FIRST_ARGUMENT_CHARACTER;
				} else if (state == READING_ARGUMENTS) {
					command_add_argument(command, content);
					state = WAITING_FIRST_ARGUMENT_CHARACTER;
				}

				input += i;
				i = 0;
				content = allocate_empty_string();
				content_size = 1;
			}
			if (input[i] == '&') {
				if (input[i + 1] == '&') {
					command->chain_type = AND;
					i++;
				} else {
					command->chain_type = BACKGROUND;
				}
				i++;
			}
			if (input[i] == '|') {
				if (input[i + 1] == '|') {
					command->chain_type = OR;
					i++;
				} else {
					command->chain_type = PIPE;
				}
				i++;
			}
		} else if (state == WAITING_FIRST_COMMAND_CHARACTER || 
							 state == WAITING_FIRST_ARGUMENT_CHARACTER) {
			// Here it transforms WAITING to READING
			state += 1;

			input += i;
			i = 0;
			content = allocate_empty_string();
			content_size = 1;
		}

		if (input[i] == '"' || input[i] == '\'') {
			content = realloc(content, (content_size + i) * sizeof(char));
			strncpy(content + content_size - 1, input, i);
			content_size += i;

			input += i;
			i = read_between_quotes(content, &content_size, input);
			if (i == -1) {
				command_free(command);
				free(content);
				return NULL;
			}
			input += i + 1;
			i = -1;
		}

		if (input[i] == '\\') {
			str_realloc_escaping_character(content, &content_size, input, i);
			input += i + 2;
			i = 0;
		}

		if (command->chain_type != NONE) {
			command->next = command_parse(input + i);

			if (command->next == NULL && command->chain_type != BACKGROUND) {
				// TODO(marcosfons): Create a strategy to "expect" a new command
				// For now only throwing a error

				// TODO(marcosfons): Introduce a hint saying what character and its position
				fprintf(stderr, "Unexpected EOF while expecting a new command");

				return NULL;
			}
			break;
		}
	} while(input[i] != '\0');

	if (command->command == NULL) {
		return NULL;
	}

	free(content);

	return command;
}

void command_free(command_t* command) {
	if (command == NULL) {
		return;
	} else if (command->next != NULL) {
		command_free(command->next);
	}

	free(command->command);

	for (int i = 0; i < command->argc; i++) {
		free(command->argv[i]);
	}
	free(command->argv);
	free(command);

	// Sanity helper
	command = NULL;
}
