#ifndef STRING_UTILS_HEADER 
#define STRING_UTILS_HEADER


// TODO(marcosfons): Implement with buffer to reduce the amount of realloc calls

typedef struct {
	int length;   /**< The length of the string */
	char* string; /**< The string itself */
} string_t;

/// @brief Creates an empty string
///
/// @returns Return the newly created string
string_t create_empty_string();

/// @brief Creates a string from the given input
///
/// @param input The input string
/// @param length The length of the input string
///
/// @return Returns the newly created string
string_t create_string_from_input(char* input, int length);

/// @brief Append string from the given input
/// 
/// @param string Pointer to the string that will grow
/// @param input The string that will be appended
/// @param length The length of the input string
///
/// @return Returns the reallocated string pointer
string_t* append_string(string_t* string, char* input, int length);

/// @brief Frees the given string
/// 
/// @param string The string that will be freed
void string_free(string_t string);

/// @brief Append string from the given input escaping the final char from it.
/// This is useful for strings that contain backslashes
///
/// The input string must be of size escaping_char_position + 2
/// 
/// @param dest The string that will be changed
/// @param input The string that will be appended
/// @param escaping_char_position The position of the escaping char "\\"
///
/// @return Returns the reallocated string pointer
string_t* append_string_escaping_final_char(string_t* dest, char* input, int escaping_char_position);

/// @brief Append string from the given input, extracting only the content between quotes
///
/// The first character of the input must be a quote. " or '
/// 
/// @param dest The string that will be changed
/// @param input The string that will be appended
///
/// @return Returns the position of the last quote or -1 if the last quote was not found
int extract_to_string_between_quotes_content(string_t* dest, char* input);


#endif
