#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>

#include "command_parser.h"
#include "../string/string_utils.h"


typedef enum {
	WAITING_FIRST_COMMAND_CHARACTER,
	READING_COMMAND,
	WAITING_FIRST_ARGUMENT_CHARACTER,
	READING_ARGUMENTS,
	WAITING_FIRST_REDIRECT_STDIN_FILE_CHARACTER,
	READING_REDIRECT_STDIN_FILE,
	WAITING_FIRST_REDIRECT_STDOUT_FILE_CHARACTER,
	READING_REDIRECT_STDOUT_FILE,
	WAITING_FIRST_REDIRECT_STDERR_FILE_CHARACTER,
	READING_REDIRECT_STDERR_FILE,
} parser_state;


command_t* command_create() {
	command_t* command = malloc(sizeof(command_t));
	if (command == NULL) {
		perror(NULL);
		exit(EXIT_FAILURE);
	}
	
	command->command = NULL;
	command->next = NULL;
	command->chain_type = NONE;
	command->argc = 0;
	command->argv = malloc(sizeof(char**));
	if (command->argv == NULL) {
		perror(NULL);
		exit(EXIT_FAILURE);
	}

	return command;
}

void command_set_command(command_t* command, char* input) {
	command->command = input;
}

void command_add_argument(command_t* command, char* argument) {
	command->argv = realloc(command->argv, (command->argc + 1) * sizeof(char*));
	if (command->argv == NULL) {
		perror(NULL);
		exit(EXIT_FAILURE);
	}

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

	string_t content = create_empty_string();

	int i = -1;

	do {
		i++;

		// Restricted chars
		if (strchr(restricted_chars, input[i]) != NONE) {
			if (state % 2 != 0) {
				append_string(&content, input, i);

				if (state == READING_COMMAND) {
					command_set_command(command, content.string);
					state = WAITING_FIRST_ARGUMENT_CHARACTER;
				} else if (state == READING_ARGUMENTS) {
					command_add_argument(command, content.string);
					state = WAITING_FIRST_ARGUMENT_CHARACTER;
				} else if (state == READING_REDIRECT_STDIN_FILE) {
					command->stdin_file_redirection = content.string;
					state = WAITING_FIRST_ARGUMENT_CHARACTER;
				} else if (state == READING_REDIRECT_STDOUT_FILE) {
					command->stdout_file_redirection = content.string;
					state = WAITING_FIRST_ARGUMENT_CHARACTER;
				}

				input += i;
				i = 0;
				content = create_empty_string();
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
		} else if (state % 2 == 0) { // State WAITING_FIRST_CHARACTER
			// Here it transforms WAITING to READING
			state += 1;

			input += i;
			i = 0;
			content = create_empty_string();
		}

		if (input[i] == '"' || input[i] == '\'') {
			append_string(&content, input, i);

			input += i;
			i = extract_to_string_between_quotes_content(&content, input);
			if (i == -1) {
				command_free(command);
				string_free(content);
				return NULL;
			}
			input += i + 1;
			i = -1;
		}

		if (input[i] == '\\') {
			append_string_escaping_final_char(&content, input, i);
			input += i + 2;
			i = -1;
		}

		if (input[i] == '=') {
			if (input[i + 1] == '>') {
				state = WAITING_FIRST_REDIRECT_STDOUT_FILE_CHARACTER;
				i++;
			} else {
				fprintf(stderr, "Syntax error: An equal sign must be followed by a >\n");
				return NULL;
			}
		}

		if (input[i] == '<') {
			if (input[i + 1] == '=') {
				state = WAITING_FIRST_REDIRECT_STDIN_FILE_CHARACTER;
				i++;
			} else {
				fprintf(stderr, "Syntax error: An < must be followed by an equal sign\n");
				return NULL;
			}
		}

		if (command->chain_type != NONE) {
			command->next = command_parse(input + i);

			if (command->next == NULL && command->chain_type != BACKGROUND) {
				/** @todo Create a strategy to "expect" a new command. For now only throwing a error and returning NULL */
				/** @todo Introduce a hint saying what character and its position */
				fprintf(stderr, "Unexpected EOF while expecting a new command");
				return NULL;
			}
			break;
		}
	} while(input[i] != '\0');

	if (command->command == NULL) {
		return NULL;
	}

	string_free(content);

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
}
