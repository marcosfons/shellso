#include <criterion/criterion.h>
#include <criterion/internal/test.h>
#include <criterion/redirect.h>

#include "../../src/shell/command_parser.c"

void redirect_all_std(void) {
    cr_redirect_stdout();
    cr_redirect_stderr();
}



// --- Command parse tests ---

Test(get_between_quotes_content, empty_string) {
	between_quotes result = get_between_quotes_content("\"\"", 0);

	cr_assert_str_eq(result.content, "");
	cr_assert_eq(result.start, 0);
	cr_assert_eq(result.end, 1);
	cr_assert_eq(result.size, 0);
}

Test(get_between_quotes_content, filled_string) {
	between_quotes result = get_between_quotes_content("\"Content inside quotes\"", 0);

	cr_assert_str_eq(result.content, "Content inside quotes");
	cr_assert_eq(result.start, 0);
	cr_assert_eq(result.end, 22);
	cr_assert_eq(result.size, 21);
}

Test(get_between_quotes_content, without_maching_quote, .init = redirect_all_std) {
	get_between_quotes_content("\"Missing last quote", 0);

	cr_assert_stderr_eq_str("Unexpected EOF while looking for matching \"\n");
}

Test(get_between_quotes_content, filled_string_with_escaping_character) {
	between_quotes result = get_between_quotes_content("\"Content \\\"inside quotes\"", 0);

	cr_assert_str_eq(result.content, "Content \"inside quotes");
	cr_assert_eq(result.start, 0);
	cr_assert_eq(result.end, 24);
	cr_assert_eq(result.size, 22);
}

Test(get_between_quotes_content, filled_string_with_multiple_escaping_character) {
	between_quotes result = get_between_quotes_content("\"Content \\\"inside\\\" quotes\"", 0);

	cr_assert_str_eq(result.content, "Content \"inside\" quotes");
	cr_assert_eq(result.start, 0);
	cr_assert_eq(result.end, 26);
	cr_assert_eq(result.size, 23);
}
//Test(get_between_quotes_content, double_quote_with_escaping_character) {
//	between_quotes result = get_between_quotes_content(
//		//"\"\\\"The future belongs to those who believe in the beauty of their dreams.\\\" Eleanor Roosevelt\"",
//		"\"The future belongs to those who believe in the beauty of their dreams. Eleanor Roosevelt\" ",
//		0
//	);
//
//	printf("ASTRING '%s'\n", result.content);
//	printf("ASTRING '%s'\n", "The future belongs to those who believe in the beauty of their dreams. Eleanor Roosevelt");
//	cr_assert_str_eq(result.content, "The future belongs to those who believe in the beauty of their dreams. Eleanor Roosevelt");
//	cr_assert_eq(result.start, 0);
//	cr_assert_eq(result.end, 22);
//	cr_assert_eq(result.size, 21);
//}



// --- Command parse tests ---

Test(join_string_with_quotes, empty_string_and_quote_elements) {
	char* result = join_string_with_quotes("", 0, 0, 0, NULL);

	cr_assert_str_eq(result, "");
}

Test(join_string_with_quotes, empty_quote_elements) {
	char* result = join_string_with_quotes("   ", 0, 2, 0, NULL);

	cr_assert_str_eq(result, "   ");
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
	printf("AAA '%s'\n", command->argv[0]);
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

Test(command_parse, echo_invalid_with_double_quotes_and_escaping_character, .init = redirect_all_std) {
	// echo out" in \"out   --> Invalid command
	command_parse("echo out\" in \\\"out");
	fflush(stderr);

	cr_assert_stderr_eq_str(
		"Unexpected EOF while looking for matching \"\n", 
		"Failed to parse invalid command escaping last quote"
	);
}
