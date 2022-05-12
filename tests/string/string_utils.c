#include <criterion/criterion.h>
#include <criterion/internal/assert.h>
#include <criterion/internal/test.h>
#include <criterion/redirect.h>
#include <stdio.h>
#include <string.h>

#include "../test_utils.h"
#include "../../src/string/string_utils.c"


Test(create_empty_string, create_empty_string) {
	string_t string = create_empty_string();

	cr_assert_eq(string.length, 1);
	cr_assert_str_eq(string.string, "");
}

Test(create_string_from_input, create_string_from_input) {
	string_t string = create_string_from_input("Hello world", 12);

	cr_assert_eq(string.length, 12);
	cr_assert_str_eq(string.string, "Hello world");
}

/// Should add one char for the null terminator, maintaning the 'D' char
Test(create_string_from_input, create_string_from_input_non_terminated) {
	char* input = strcpy(malloc(12 * sizeof(char)), "Hello world");
	input[11] = 'D'; // Different of '\0'

	string_t string = create_string_from_input(input, 12);

	cr_assert_eq(string.length, 13);
	cr_assert_str_eq(string.string, "Hello worldD");
}

Test(append_string, append_world_string) {
	char* input = strcpy(malloc(6 * sizeof(char)), "Hello");
	input[5] = '\0';

	string_t string = create_string_from_input(input, 6);
	// Sanity check
	cr_assert_eq(string.length, 6);
	cr_assert_str_eq(string.string, "Hello");

	append_string(&string, " world", 7);

	cr_assert_eq(string.length, 12);
	cr_assert_str_eq(string.string, "Hello world");
}

Test(append_string, append_world_string_not_null_terminated) {
	char* input = strcpy(malloc(7 * sizeof(char)), " world");
	string_t string = create_string_from_input("Hello", 6);
	// Sanity check
	cr_assert_eq(string.length, 6);
	cr_assert_str_eq(string.string, "Hello");

	append_string(&string, input, 7);

	cr_assert_eq(string.length, 12);
	cr_assert_str_eq(string.string, "Hello world");
}




Test(append_string_escaping_final_char, simple_str_realloc_escaping_character) {
	string_t dest = create_empty_string();
	append_string_escaping_final_char(&dest, "Testing\\\"", 7);

	cr_assert_str_eq(dest.string, "Testing\"");
	cr_assert_eq(dest.length, 9);
}

Test(append_string_escaping_final_char, str_escaping_character_with_content) {
	string_t dest = create_string_from_input("Hello ", 6);
	append_string_escaping_final_char(&dest, "Testing\\\"", 7);

	cr_assert_str_eq(dest.string, "Hello Testing\"");
	cr_assert_eq(dest.length, 15);
}

Test(extract_to_string_between_quotes_content, hello_world_between_quotes) {
	string_t dest = create_string_from_input("Hello", 6);
	char* final = "' world!'";
	int final_quote = extract_to_string_between_quotes_content(&dest, final);

	cr_assert_str_eq(dest.string, "Hello world!");
	cr_assert_eq(final[final_quote], '\'');
	cr_assert_eq(dest.length, 13);
}

Test(extract_to_string_between_quotes_content, empty_string) {
	string_t dest = create_empty_string();
	char* final = "''";
	int final_quote = extract_to_string_between_quotes_content(&dest, final);

	cr_assert_str_eq(dest.string, "");
	cr_assert_eq(final_quote, 1);
	cr_assert_eq(final[final_quote], '\'');
	cr_assert_eq(dest.length, 1);
}

Test(extract_to_string_between_quotes_content, double_quote_empty_string) {
	string_t dest = create_empty_string();
	char* final = "''";
	int final_quote = extract_to_string_between_quotes_content(&dest, final);

	cr_assert_str_eq(dest.string, "");
	cr_assert_eq(final_quote, 1);
	cr_assert_eq(final[final_quote], '\'');
	cr_assert_eq(dest.length, 1);
}

Test(extract_to_string_between_quotes_content, hello_world_double_quotes) {
	string_t dest = create_string_from_input("Hello", 6);
	char* final = "\" world!\"";
	int final_quote = extract_to_string_between_quotes_content(&dest, final);

	cr_assert_str_eq(dest.string, "Hello world!");
	cr_assert_eq(final[final_quote], '"');
	cr_assert_eq(dest.length, 13);
}

Test(extract_to_string_between_quotes_content, hello_world_double_quotes_escaped) {
	string_t dest = create_string_from_input("Hello", 6);
	char* final = "\" \\\"world\\\"!\"";
	int final_quote = extract_to_string_between_quotes_content(&dest, final);

	cr_assert_str_eq(dest.string, "Hello \"world\"!");
	cr_assert_eq(final[final_quote], '"');
	cr_assert_eq(dest.length, 15);
}

Test(extract_to_string_between_quotes_content, hello_world_escaped_double_quotes) {
	string_t dest = create_string_from_input("Hello", 6);
	char* final = "\" \\\"world!\"";
	int final_quote = extract_to_string_between_quotes_content(&dest, final);

	cr_assert_str_eq(dest.string, "Hello \"world!");
	cr_assert_eq(final[final_quote], '"');
	cr_assert_eq(dest.length, 14);
}

Test(extract_to_string_between_quotes_content, hello_world_double_quotes_escaped2) {
	string_t dest = create_string_from_input("Hello", 6);
	char* final = "\" \\\"\\\"world\\\"!\"";
	int final_quote = extract_to_string_between_quotes_content(&dest, final);

	cr_assert_str_eq(dest.string, "Hello \"\"world\"!");
	cr_assert_eq(final[final_quote], '"');
	cr_assert_eq(dest.length, 16);
}

Test(extract_to_string_between_quotes_content, hello_world_double_quotes_escaped3) {
	string_t dest = create_string_from_input("Hello", 6);
	char* final = "\" \\\" \\\" \\\"world\\\"!\"";
	int final_quote = extract_to_string_between_quotes_content(&dest, final);

	cr_assert_str_eq(dest.string, "Hello \" \" \"world\"!");
	cr_assert_eq(final[final_quote], '"');
	cr_assert_eq(dest.length, 19);
}

Test(extract_to_string_between_quotes_content, missing_last_quote, .init=redirect_stderr) {
	string_t dest = create_string_from_input("Hello", 6);
	int final_quote = extract_to_string_between_quotes_content(&dest, "' world!");

	cr_assert_eq(final_quote, -1);
	cr_assert_stderr_eq_str(
		"Unexpected EOF while looking for matching '\n", 
		"Should not pass, missing last quote"
	);
}
