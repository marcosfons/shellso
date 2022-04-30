#ifndef STRING_UTILS_HEADER 
#define STRING_UTILS_HEADER

// TODO(marcosfons): Implement with buffer to reduce the amount of realloc calls

typedef struct {
	int length;
	char* string;
} string_t;


string_t create_empty_string();

string_t create_string_from_input(char* input, int length);

string_t* append_string(string_t* string, char* input, int length);

void string_free(string_t string);


string_t* append_string_escaping_final_char(string_t* dest, char* input, int escaping_char_position);
// TODO(marcosfons): Fix wrong doc
/* Extract the content between single quotes inside input into dest string
 *
 * Arguments:
 * 	char* dest
 * 	int size 		the size of the dest string before concatenating. dest[size - 1] == '\0'
 *	char* input the input string, the initial character must be " or '
 *
 * returns a int indicating the position of the last quote, if the 
 * last quote was not found it returns -1
 *
 */
int extract_to_string_between_quotes_content(string_t* dest, char* input);


#endif
