#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "command_parser.h"


typedef enum parser_state {
	WAITING_FIRST_COMMAND_CHARACTER,
	READING_COMMAND,
	WAITING_FIRST_ARGUMENT_CHARACTER,
	READING_ARGUMENTS
} parser_state;


typedef struct {
	char* content;
	int start;
	int end;
	int size; // Size is different of end - start, cause in double quotes still has escaping characters
} between_quotes;


static char* allocate_string(char* input, int start, int end) {
	int null_terminator_missing = input[end] != '\0';

	char* result = malloc(((end + null_terminator_missing) - start) * sizeof(char));
		
	strncpy(result, input + start, end - start);
	if (null_terminator_missing) {
		result[end + 1] = '\0';
	}

	return result;
}


/* Returns the content between quotes of a string
 *
 * Supports single and double quotes
 * In double quotes escaping characters works as well
 *
 * Arguments:
 *	char* input
 *	int start  The position of the first quote. Must be inclusive
 *
 */
static between_quotes get_between_quotes_content(char* input, int start) {
	char quote_type = input[start]; // Can be " or '
	int i;
	int size = 0;
	int position_after_escaped_char = start + 1;
	char* content = malloc(sizeof(char));

	for (i = start + 1; input[i] != quote_type && input[i] != '\0'; i++) {
		size++;
		if (quote_type == '"' && input[i] == '\\') {
			i++; // Jumps the escaping character
			content = realloc(content, (size + 1) * sizeof(char));

			strcat(content, allocate_string(input, position_after_escaped_char, i));

			content[size - 1] = input[i]; // Replace the escaping character

			position_after_escaped_char = i + 1;
		}
	}

	// TODO(marcosfons): Handle error return
	if (input[i] == '\0') {
		fprintf(stderr, "Unexpected EOF while looking for matching %c\n", quote_type);
	}

	content = realloc(content, (size + 1) * sizeof(char));
	strcat(content, allocate_string(input, position_after_escaped_char, i));

	between_quotes extracted;
	extracted.start = start;
	extracted.end = i;
	extracted.size = size;
	extracted.content = content;

	return extracted;
}


char* join_string_with_quotes(char* input, int start, int end, int quotes_count, between_quotes* elements) {
	char* content = malloc(sizeof(char));
	content[0] = '\0';

	for (int i = 0; i < quotes_count; i++) {
		int current_start = i == 0 ? start : elements[i - 1].end;
	
		int total_size = elements[i].end - current_start;

		content = realloc(content, (total_size + 1) * sizeof(char));

		strncat(content, input + current_start, elements[i].start);

		strncat(content, input + current_start, elements[i].start);

	}

	return content;
}


command_t* command_parse(char* input) {
	if (input == NULL || input[0] == '\0') {
		return NULL;
	}
	command_t* command = malloc(sizeof(command_t));

	command->command = NULL;
	command->argc = 0;
	command->argv = malloc(sizeof(char**));
	
	// TODO(marcosfons): Add => <= < >
	//char* restricted_chars = " &|$";

	parser_state state = WAITING_FIRST_COMMAND_CHARACTER;

	int previous = 0;
	int i = -1;

	int quotes_count = 0;
	between_quotes* quotes_elements = malloc(sizeof(between_quotes*));

	char* last_content = malloc(sizeof(char));

	// Using do while to handle null terminator cases inside the loop
	do {
		i++;

		if (input[i] == ' ' || input[i] == '\0') {
			if (state == READING_COMMAND) {
				command->command = allocate_string(input, previous, i);

				state = WAITING_FIRST_ARGUMENT_CHARACTER;
				previous = i;

				last_content = realloc(last_content, sizeof(char));
			} else if (state == READING_ARGUMENTS) {
				command->argv = realloc(command->argv, (command->argc + 1) * sizeof(char*));

				if (quotes_count > 0) {
					for (int j = previous; j < i; j++) {
						
					}
					command->argv[command->argc] = quotes_elements[0].content;
				} else {
					command->argv[command->argc] = allocate_string(input, previous, i);
				}

				command->argc += 1;
				quotes_count = 0;
				quotes_elements = malloc(sizeof(between_quotes*));
				previous = i;

				state = WAITING_FIRST_ARGUMENT_CHARACTER;

				last_content = realloc(last_content, sizeof(char));
			}
		} else {
			if (state == WAITING_FIRST_COMMAND_CHARACTER) {
				state = READING_COMMAND;
				previous = i;
			} else if (state == WAITING_FIRST_ARGUMENT_CHARACTER) {
				state = READING_ARGUMENTS;
				previous = i;
			}
		}

		// Handle double and single quotes
		if (input[i] == '"' || input[i] == '\'') {

			quotes_elements = realloc(quotes_elements, (quotes_count + 1) * sizeof(between_quotes));
			quotes_elements[quotes_count] = get_between_quotes_content(input, i);
			quotes_count += 1;

			int size = quotes_elements[quotes_count - 1].size;
			int start = quotes_elements[quotes_count - 1].start;
			char* content = quotes_elements[quotes_count - 1].content;
			
			int last_content_size = strlen(last_content);
			last_content = realloc(last_content, (last_content_size + size + 1)* sizeof(char));
			printf("Será: '%s'\n", last_content);
			printf("Actual lwlSerá: '%s'\n", content);

			if (quotes_count > 1) {
				
				strncat(last_content + , input + i, start - i);
			} else {
				strncat(last_content, input + i, start - i);
			}

			strcat(last_content + last_content_size, content);
			printf("depois lwlSerá: '%s'\n", last_content);

			i = quotes_elements[quotes_count - 1].end;
		}
	} while(input[i] != '\0');

	if (state == WAITING_FIRST_COMMAND_CHARACTER) {
		return NULL;
	}
	
	////printf("commmand: '%s'\n", command->command);
	////printf("PRIMERIA: '%s'\n", command->argv[0]);
	////printf("SEGUNDA: '%s'\n", command->argv[1]);
	return command;
}


//void free_command(command_t* command) {
//	if (command == NULL) {
//		return;
//	}
//
//	free(command->command);
//
//	for (int i = 0; i < command->argc; i++) {
//		free(command->argv[i]);
//	}
//
//	// TODO(marcosfons): Fix compilation warning
//	//free(command->argv);
//}
//
//// TODO(marcosfons): Finish free_command_chain
//void free_command_chain(command_t* command) {
//	
//}
