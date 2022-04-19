#include <criterion/criterion.h>
#include <criterion/internal/assert.h>
#include <criterion/internal/test.h>
#include <criterion/redirect.h>
#include <stdio.h>
#include <string.h>

#include "../../src/shell/command_parser.c"

void redirect_stderr(void) {
	cr_redirect_stderr();
}

void redirect_all_std(void) {
    cr_redirect_stdout();
    cr_redirect_stderr();
}



// --- Command parse tests ---


Test(str_realloc_escaping_character, simple_str_realloc_escaping_character) {
	char* dest = malloc(sizeof(char));
	dest[0] = '\0';
	int size = 1;

	str_realloc_escaping_character(dest, &size, "Testing\\\"", 7);

	cr_assert_str_eq(dest, "Testing\"");
	cr_assert_eq(size, 9);
}

Test(str_realloc_escaping_character, str_escaping_character_with_content) {
	char* dest = strcpy(malloc(7), "Hello ");
	int size = 7;

	str_realloc_escaping_character(dest, &size, "Testing\\\"", 7);

	cr_assert_str_eq(dest, "Hello Testing\"");
	cr_assert_eq(size, 15);
}

Test(read_between_quotes, hello_world) {
	char* dest = strcpy(malloc(6), "Hello");
	int size = 6;
	char* final = "' world!'";
	int final_quote = read_between_quotes(dest, &size, final);

	cr_assert_str_eq(dest, "Hello world!");
	cr_assert_eq(final[final_quote], '\'');
	cr_assert_eq(size, 13);
}

Test(read_between_quotes, empty_string) {
	char* dest = strcpy(malloc(1), "");
	int size = 1;
	char* final = "''";
	int final_quote = read_between_quotes(dest, &size, final);

	cr_assert_str_eq(dest, "");
	cr_assert_eq(final_quote, 1);
	cr_assert_eq(final[final_quote], '\'');
	cr_assert_eq(size, 1);
}

Test(read_between_quotes, double_quote_empty_string) {
	char* dest = strcpy(malloc(1), "");
	int size = 1;
	char* final = "''";
	int final_quote = read_between_quotes(dest, &size, final);

	cr_assert_str_eq(dest, "");
	cr_assert_eq(final_quote, 1);
	cr_assert_eq(final[final_quote], '\'');
	cr_assert_eq(size, 1);
}

Test(read_between_quotes, hello_world_double_quotes) {
	char* dest = strcpy(malloc(6), "Hello");
	int size = 6;
	char* final = "\" world!\"";
	int final_quote = read_between_quotes(dest, &size, final);

	cr_assert_str_eq(dest, "Hello world!");
	cr_assert_eq(final[final_quote], '"');
	cr_assert_eq(size, 13);
}

Test(read_between_quotes, hello_world_double_quotes_escaped) {
	char* dest = strcpy(malloc(6), "Hello");
	int size = 6;
	char* final = "\" \\\"world\\\"!\"";
	int final_quote = read_between_quotes(dest, &size, final);

	cr_assert_str_eq(dest, "Hello \"world\"!");
	cr_assert_eq(final[final_quote], '"');
	cr_assert_eq(size, 15);
}

Test(read_between_quotes, hello_world_escaped_double_quotes) {
	char* dest = strcpy(malloc(6), "Hello");
	int size = 6;
	char* final = "\" \\\"world!\"";

	int final_quote = read_between_quotes(dest, &size, final);

	cr_assert_str_eq(dest, "Hello \"world!");
	cr_assert_eq(final[final_quote], '"');
	cr_assert_eq(size, 14);
}

Test(read_between_quotes, hello_world_double_quotes_escaped2) {
	char* dest = strcpy(malloc(6), "Hello");
	int size = 6;
	char* final = "\" \\\"\\\"world\\\"!\"";
	int final_quote = read_between_quotes(dest, &size, final);

	cr_assert_str_eq(dest, "Hello \"\"world\"!");
	cr_assert_eq(final[final_quote], '"');
	cr_assert_eq(size, 16);
}

Test(read_between_quotes, hello_world_double_quotes_escaped3) {
	char* dest = strcpy(malloc(6), "Hello");
	int size = 6;
	char* final = "\" \\\" \\\" \\\"world\\\"!\"";
	int final_quote = read_between_quotes(dest, &size, final);

	cr_assert_str_eq(dest, "Hello \" \" \"world\"!");
	cr_assert_eq(final[final_quote], '"');
	cr_assert_eq(size, 19);
}

Test(read_between_quotes, missing_last_quote) {
	char* dest = strcpy(malloc(6), "Hello");
	int size = 6;
	int final_quote = read_between_quotes(dest, &size, "' world!");

	cr_assert_eq(final_quote, -1);
	cr_assert_eq(size, 6);
}




// --- Command parse tests ---

Test(command_parse, null_command) {
	command_t* command = command_parse(NULL);

	cr_assert_null(command);
}

Test(command_parse, empty_command) {
	command_t* command = command_parse("");

	cr_assert_null(command);
}

Test(command_parse, ls_command) {
	command_t* command = command_parse("ls");

	cr_assert_str_eq(command->command, "ls");
	cr_assert_eq(command->argc, 0);
	cr_assert_null(command->argv[0]);
}

Test(command_parse, empty_command_with_spaces) {
	command_t* command = command_parse("    ");

	cr_assert_null(command);
}

Test(command_parse, ls_with_argument) {
	command_t* command = command_parse("ls -l");

	cr_assert_str_eq(command->command, "ls");
	cr_assert_eq(command->argc, 1);
	cr_assert_str_eq(command->argv[0], "-l");
}

Test(command_parse, ls_with_leading_spaces) {
	command_t* command = command_parse("  ls");

	cr_assert_str_eq(command->command, "ls");
	cr_assert_eq(command->argc, 0);
	cr_assert_null(command->argv[0]);
}

Test(command_parse, ls_with_trailing_spaces) {
	command_t* command = command_parse("ls  ");

	cr_assert_str_eq(command->command, "ls");
	cr_assert_eq(command->argc, 0);
	cr_assert_null(command->argv[0]);
}

Test(command_parse, ls_with_spaces_between_arguments) {
	command_t* command = command_parse("ls  -l  ");

	cr_assert_str_eq(command->command, "ls");
	cr_assert_eq(command->argc, 1);
	cr_assert_str_eq(command->argv[0], "-l");
}

Test(command_parse, git_multiple_arguments_without_quotes) {
	command_t* command = command_parse("git commit -m Message");

	cr_assert_str_eq(command->command, "git");
	cr_assert_eq(command->argc, 3);
	cr_assert_str_eq(command->argv[0], "commit");
	cr_assert_str_eq(command->argv[1], "-m");
	cr_assert_str_eq(command->argv[2], "Message");
}

Test(command_parse, git_multiple_arguments) {
	command_t* command = command_parse("git commit -m 'Message'");

	cr_assert_str_eq(command->command, "git");
	cr_assert_eq(command->argc, 3);
	cr_assert_str_eq(command->argv[0], "commit");
	cr_assert_str_eq(command->argv[1], "-m");
	cr_assert_str_eq(command->argv[2], "Message");
}

Test(command_parse, git_multiple_arguments_with_quotes_and_spaces) {
	command_t* command = command_parse("git commit -m 'Message with spaces'");

	cr_assert_str_eq(command->command, "git");
	cr_assert_eq(command->argc, 3);
	cr_assert_str_eq(command->argv[0], "commit");
	cr_assert_str_eq(command->argv[1], "-m");
	cr_assert_str_eq(command->argv[2], "Message with spaces");
}

Test(command_parse, echo_valid) {
	command_t* command = command_parse("echo test'test'");

	cr_assert_str_eq(command->command, "echo");
	cr_assert_eq(command->argc, 1);
	cr_assert_str_eq(command->argv[0], "testtest");
}

Test(command_parse, echo_valid_with_quotes) {
	command_t* command = command_parse("echo out' in 'out");

	cr_assert_str_eq(command->command, "echo");
	cr_assert_eq(command->argc, 1);
	cr_assert_str_eq(command->argv[0], "out in out");
}

Test(command_parse, echo_valid_with_single_quote_and_escaping_character) {
	command_t* command = command_parse("echo out' in \\'out");

	cr_assert_str_eq(command->command, "echo");
	cr_assert_eq(command->argc, 1);
	cr_assert_str_eq(command->argv[0], "out in \\out");
}

Test(command_parse, echo_valid_with_double_quotes_and_escaping_character) {
	// echo out" in \""out   --> Valid command
	command_t* command = command_parse("echo out\" in \\\"\"out");

	cr_assert_str_eq(command->command, "echo");
	cr_assert_eq(command->argc, 1);
	cr_assert_str_eq(command->argv[0], "out in \"out");
}

Test(command_parse, echo_valid_with_double_quotes) {
	// echo out" in \""out   --> Valid command
	command_t* command = command_parse("echo out\" in \"out\" in \"");

	cr_assert_str_eq(command->command, "echo");
	cr_assert_eq(command->argc, 1);
	cr_assert_str_eq(command->argv[0], "out in out in ");
}

Test(command_parse, simple_echo_command) {
	command_t* command = command_parse("echo \"Testando\" 'Teste'");

	cr_assert_str_eq(command->command, "echo");
	cr_assert_eq(command->argc, 2);
	cr_assert_str_eq(command->argv[0], "Testando");
	cr_assert_str_eq(command->argv[1], "Teste");
}

Test(command_parse, simple_echo_pipe_command_chain) {
	command_t* command = command_parse("echo test | echo 123123");

	cr_assert_str_eq(command->command, "echo");
	cr_assert_eq(command->argc, 1);
	cr_assert_str_eq(command->argv[0], "test");
	cr_assert_eq(command->chain_type, PIPE);

	cr_assert_str_eq(command->next->command, "echo");
	cr_assert_eq(command->next->argc, 1);
	cr_assert_str_eq(command->next->argv[0], "123123");
}

Test(command_parse, simple_echo_or_command_chain) {
	command_t* command = command_parse("echo test || echo 123123");

	cr_assert_str_eq(command->command, "echo");
	cr_assert_eq(command->argc, 1);
	cr_assert_str_eq(command->argv[0], "test");
	cr_assert_eq(command->chain_type, OR);

	cr_assert_str_eq(command->next->command, "echo");
	cr_assert_eq(command->next->argc, 1);
	cr_assert_str_eq(command->next->argv[0], "123123");
}

Test(command_parse, simple_echo_pipe_command_chain_without_spaces) {
	command_t* command = command_parse("echo test|echo 123123");

	cr_assert_str_eq(command->command, "echo");
	cr_assert_eq(command->argc, 1);
	cr_assert_str_eq(command->argv[0], "test");
	cr_assert_eq(command->chain_type, PIPE);

	cr_assert_str_eq(command->next->command, "echo");
	cr_assert_eq(command->next->argc, 1);
	cr_assert_str_eq(command->next->argv[0], "123123");
}

Test(command_parse, and_command_chain) {
	command_t* command = command_parse("echo test test1 &&cat 123123");

	cr_assert_str_eq(command->command, "echo");
	cr_assert_eq(command->argc, 2);
	cr_assert_str_eq(command->argv[0], "test");
	cr_assert_str_eq(command->argv[1], "test1");
	cr_assert_eq(command->chain_type, AND);

	cr_assert_str_eq(command->next->command, "cat");
	cr_assert_eq(command->next->argc, 1);
	cr_assert_str_eq(command->next->argv[0], "123123");
}

Test(command_parse, command_chain_with_escaping_character) {
	command_t* command = command_parse("cat test\\ test1 & echo 123123");

	cr_assert_str_eq(command->command, "cat");
	cr_assert_eq(command->argc, 1);
	cr_assert_str_eq(command->argv[0], "test test1");
	cr_assert_eq(command->chain_type, BACKGROUND);

	cr_assert_str_eq(command->next->command, "echo");
	cr_assert_eq(command->next->argc, 1);
	cr_assert_str_eq(command->next->argv[0], "123123");
}

Test(command_parse, echo_invalid_with_double_quotes_and_escaping_character, .init = redirect_stderr) {
	command_parse("echo out\" in \\\"out");

	cr_assert_stderr_eq_str(
		"Unexpected EOF while looking for matching \"\n",
		"Failed to parse invalid command escaping last quote"
	);
}
Test(command_parse, echo_invalid_with_quotes_and_escaping_character, .init = redirect_stderr) {
	command_parse("\'echo test");

	cr_assert_stderr_eq_str(
		"Unexpected EOF while looking for matching \'\n",
		"Failed to parse invalid command escaping last quote"
	);
}
