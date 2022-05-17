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



command* command_parse(char* input) {
	// @todo Redo this conditional. It only works for # in the beginning of line
	if (input == NULL || input[0] == '#' || input[0] == '\n' || input[0] == '\0') {
		return NULL;
	}

	command* cmd = command_create();

	// Using => to redirect stdout '>' isn't a restricted char
	const char* restricted_chars = "&| =<\0";

	string_t content = create_empty_string();
	parser_state state = WAITING_FIRST_COMMAND_CHARACTER;
	for (int i = 0; i == 0 || input[i - 1] != '\0'; i++) {

		if (strchr(restricted_chars, input[i]) != NONE) {
			if (state % 2 != 0) { // State READING
				append_string(&content, input, i);

				if (state == READING_COMMAND) {
					command_set_command(cmd, content.string);
				} else if (state == READING_ARGUMENTS) {
					command_add_argument(cmd, content.string);
				} else if (state == READING_REDIRECT_STDIN_FILE) {
					cmd->stdin_file_redirection = content.string;
				} else if (state == READING_REDIRECT_STDOUT_FILE) {
					cmd->stdout_file_redirection = content.string;
				}

				state = WAITING_FIRST_ARGUMENT_CHARACTER;
				input += i;
				i = 0;
				content = create_empty_string();
			}
		} else if (state % 2 == 0) { // State WAITING_FIRST_CHARACTER
			// Here it transforms WAITING to READING
			state += 1;

			input += i;
			i = 0;
			if (content.string != NULL) {
				string_free(content);
			}
			content = create_empty_string();
		}

		if (input[i] == '"' || input[i] == '\'') {
			append_string(&content, input, i);

			input += i;
			i = extract_to_string_between_quotes_content(&content, input);
			if (i == -1) {
				command_free(cmd);
				string_free(content);
				return NULL;
			}
			input += i + 1; // Jumps the last quote
			i = -1;
		}

		if (input[i] == '\\') {
			append_string_escaping_final_char(&content, input, i);
			input += i + 2; // Jumps \ and the escaped char
			i = -1;
		}

		if (input[i] == '=') {
			if (input[i + 1] == '>') {
				state = WAITING_FIRST_REDIRECT_STDOUT_FILE_CHARACTER;
				i++;
			} else {
				fprintf(stderr, "Syntax error: An equal sign must be followed by a >\n");
				command_free(cmd);
				string_free(content);
				return NULL;
			}
		}

		if (input[i] == '<') {
			if (input[i + 1] == '=') {
				state = WAITING_FIRST_REDIRECT_STDIN_FILE_CHARACTER;
				i++;
			} else {
				fprintf(stderr, "Syntax error: An < must be followed by an equal sign\n");
				command_free(cmd);
				string_free(content);
				return NULL;
			}
		}

		if (input[i] == '&' || input[i] == '|') {

			if (input[i] == '&') {
				if (input[i + 1] == '&') {
					cmd->chain_type = AND;
					i++;
				} else {
					cmd->chain_type = BACKGROUND;
				}
			}
			if (input[i] == '|') {
				if (input[i + 1] == '|') {
					cmd->chain_type = OR;
					i++;
				} else {
					cmd->chain_type = PIPE;
				}
			}

			cmd->next = command_parse(input + i + 1);
			if (cmd->next == NULL && cmd->chain_type != BACKGROUND) {
				/** @todo Create a strategy to "expect" a new command. For now only throwing a error and returning NULL */
				/** @todo Introduce a hint saying what character and its position */
				fprintf(stderr, "Unexpected EOF while expecting a new command");
				command_free(cmd);
				string_free(content);
				return NULL;
			}
			break;
		}
	}
	
	string_free(content);

	if (cmd->argv[0] == NULL) {
		command_free(cmd);
		return NULL;
	}

	return cmd;
}
