#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "string_utils.h"


/** Try to allocate some space (size) in memory but exits in some unknown failure */
static void* malloc_or_exit(size_t size, char* reason) {
	void* pointer = malloc(size);
	if (pointer == NULL) {
		perror(reason);
		exit(EXIT_FAILURE);
	}

	return pointer;
}

/** Try to reallocate some space (size) in memory for pointer but exits in some unknown failure */
static void* realloc_or_exit(void* pointer, size_t size, char* reason) {
	pointer = realloc(pointer, size);
	if (pointer == NULL) {
		perror(reason);
		return NULL;
	}

	return pointer;
}

string_t create_empty_string() {
	string_t string;

	string.length = 1;
	string.string = malloc_or_exit(sizeof(char), "Error allocating empty string");
	string.string[string.length - 1] = '\0';

	return string;
}

string_t create_string_from_input(char* input, int length) {
	string_t string;

	string.length = length + (input[length - 1] != '\0'); // Adds one if its not null terminated
	string.string = malloc_or_exit(string.length * sizeof(char), "Error allocating string");

	strncpy(string.string, input, length);

	string.string[string.length - 1] = '\0';

	return string;
}

string_t* append_string(string_t* string, char* input, int length) {
	if (length == 0) {
		return string;
	}
	int is_null_terminated = input[length - 1] == '\0';
	
	int new_length = string->length + length - is_null_terminated;
	string->string = realloc_or_exit(string->string, new_length * sizeof(char), "Error reallocating string");

	strncpy(string->string + string->length - 1, input, length); // Check if it needs to subtract is_null_terminated

	string->length = new_length;
	string->string[string->length - 1] = '\0';

	return string;
}

string_t* append_string_escaping_final_char(string_t* dest, char* input, int escaping_char_position) {
	dest = append_string(dest, input, escaping_char_position + 1);
	dest->string[dest->length - 2] = input[escaping_char_position + 1];
	
	return dest;
}

int extract_to_string_between_quotes_content(string_t* dest, char* input) {
	char quote_type = input[0]; // will be " or '
	assert(quote_type == '"' || quote_type == '\'');
	input += 1; // shift the quote char
	int i = 0;

	if (quote_type == '\'') {
		for (; input[i] != quote_type && input[i] != '\0'; i++) {}
		if (i != 0) {
			append_string(dest, input, i);
		}
	} else {
		int j = 0;
		for (; input[j] != quote_type && input[j] != '\0'; j++) {
			if (input[j] == '\\') {
				append_string_escaping_final_char(dest, input, j);
				input += j + 2;
				i += j + 2;
				j = -1;
			}
		}
		i += j;
		append_string(dest, input, j);
	}
	
	// Reached the end of the string without finding the end quote
	if (input[i] == '\0') {
		fprintf(stderr, "Unexpected EOF while looking for matching %c\n", quote_type);
		return -1;
	}

	return i + 1;
}

void string_free(string_t string) {
	free(string.string);
	string.length = 0;
}
