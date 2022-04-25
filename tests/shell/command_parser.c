#include <criterion/criterion.h>
#include <criterion/internal/assert.h>
#include <criterion/internal/test.h>
#include <criterion/redirect.h>
#include <stdio.h>
#include <string.h>

#include "../test_utils.h"
#include "../../src/shell/command.c"
#include "../../src/shell/command_parser.c"

// --- Command parse tests ---

Test(command_parse, null_command) {
	command* cmd = command_parse(NULL);

	cr_assert_null(cmd);
}

Test(command_parse, empty_command) {
	command* cmd = command_parse("");

	cr_assert_null(cmd);
}

Test(command_parse, ls_command) {
	command* cmd = command_parse("ls");

	cr_assert_str_eq(cmd->command, "ls");
	cr_assert_eq(cmd->argc, 0);
	cr_assert_null(cmd->argv[0]);
}

Test(command_parse, empty_command_with_spaces) {
	command* cmd = command_parse("    ");

	cr_assert_null(cmd);
}

Test(command_parse, ls_with_argument) {
	command* cmd = command_parse("ls -l");

	cr_assert_str_eq(cmd->command, "ls");
	cr_assert_eq(cmd->argc, 1);
	cr_assert_str_eq(cmd->argv[0], "-l");
}

Test(command_parse, ls_with_leading_spaces) {
	command* cmd = command_parse("  ls");

	cr_assert_str_eq(cmd->command, "ls");
	cr_assert_eq(cmd->argc, 0);
	cr_assert_null(cmd->argv[0]);
}

Test(command_parse, ls_with_trailing_spaces) {
	command* cmd = command_parse("ls  ");

	cr_assert_str_eq(cmd->command, "ls");
	cr_assert_eq(cmd->argc, 0);
	cr_assert_null(cmd->argv[0]);
}

Test(command_parse, ls_with_spaces_between_arguments) {
	command* cmd = command_parse("ls  -l  ");

	cr_assert_str_eq(cmd->command, "ls");
	cr_assert_eq(cmd->argc, 1);
	cr_assert_str_eq(cmd->argv[0], "-l");
}

Test(command_parse, git_multiple_arguments_without_quotes) {
	command* cmd = command_parse("git commit -m Message");

	cr_assert_str_eq(cmd->command, "git");
	cr_assert_eq(cmd->argc, 3);
	cr_assert_str_eq(cmd->argv[0], "commit");
	cr_assert_str_eq(cmd->argv[1], "-m");
	cr_assert_str_eq(cmd->argv[2], "Message");
}

Test(command_parse, git_multiple_arguments) {
	command* cmd = command_parse("git commit -m 'Message'");

	cr_assert_str_eq(cmd->command, "git");
	cr_assert_eq(cmd->argc, 3);
	cr_assert_str_eq(cmd->argv[0], "commit");
	cr_assert_str_eq(cmd->argv[1], "-m");
	cr_assert_str_eq(cmd->argv[2], "Message");
}

Test(command_parse, git_multiple_arguments_with_quotes_and_spaces) {
	command* cmd = command_parse("git commit -m 'Message with spaces'");

	cr_assert_str_eq(cmd->command, "git");
	cr_assert_eq(cmd->argc, 3);
	cr_assert_str_eq(cmd->argv[0], "commit");
	cr_assert_str_eq(cmd->argv[1], "-m");
	cr_assert_str_eq(cmd->argv[2], "Message with spaces");
}

Test(command_parse, echo_valid) {
	command* cmd = command_parse("echo test'test'");

	cr_assert_str_eq(cmd->command, "echo");
	cr_assert_eq(cmd->argc, 1);
	cr_assert_str_eq(cmd->argv[0], "testtest");
}

Test(command_parse, echo_valid_with_quotes) {
	command* cmd = command_parse("echo out' in 'out");

	cr_assert_str_eq(cmd->command, "echo");
	cr_assert_eq(cmd->argc, 1);
	cr_assert_str_eq(cmd->argv[0], "out in out");
}

Test(command_parse, echo_valid_with_single_quote_and_escaping_character) {
	command* cmd = command_parse("echo out' in \\'out");

	cr_assert_str_eq(cmd->command, "echo");
	cr_assert_eq(cmd->argc, 1);
	cr_assert_str_eq(cmd->argv[0], "out in \\out");
}

Test(command_parse, echo_valid_with_double_quotes_and_escaping_character) {
	// echo out" in \""out   --> Valid command
	command* cmd = command_parse("echo out\" in \\\"\"out");

	cr_assert_str_eq(cmd->command, "echo");
	cr_assert_eq(cmd->argc, 1);
	cr_assert_str_eq(cmd->argv[0], "out in \"out");
}

Test(command_parse, echo_valid_with_double_quotes) {
	command* cmd = command_parse("echo out\" in \"out\" in \"");

	cr_assert_str_eq(cmd->command, "echo");
	cr_assert_eq(cmd->argc, 1);
	cr_assert_str_eq(cmd->argv[0], "out in out in ");
}

Test(command_parse, simple_echo_command) {
	command* cmd = command_parse("echo \"Testing\" 'Test'");

	cr_assert_str_eq(cmd->command, "echo");
	cr_assert_eq(cmd->argc, 2);
	cr_assert_str_eq(cmd->argv[0], "Testing");
	cr_assert_str_eq(cmd->argv[1], "Test");
}

Test(command_parse, simple_echo_pipe_command_chain) {
	command* cmd = command_parse("echo test | echo 123123");

	cr_assert_str_eq(cmd->command, "echo");
	cr_assert_eq(cmd->argc, 1);
	cr_assert_str_eq(cmd->argv[0], "test");
	cr_assert_eq(cmd->chain_type, PIPE);

	cr_assert_str_eq(cmd->next->command, "echo");
	cr_assert_eq(cmd->next->argc, 1);
	cr_assert_str_eq(cmd->next->argv[0], "123123");
}

Test(command_parse, simple_echo_or_command_chain) {
	command* cmd = command_parse("echo test || echo 123123");

	cr_assert_str_eq(cmd->command, "echo");
	cr_assert_eq(cmd->argc, 1);
	cr_assert_str_eq(cmd->argv[0], "test");
	cr_assert_eq(cmd->chain_type, OR);

	cr_assert_str_eq(cmd->next->command, "echo");
	cr_assert_eq(cmd->next->argc, 1);
	cr_assert_str_eq(cmd->next->argv[0], "123123");
}

Test(command_parse, simple_echo_pipe_command_chain_without_spaces) {
	command* cmd = command_parse("echo test|echo 123123");

	cr_assert_str_eq(cmd->command, "echo");
	cr_assert_eq(cmd->argc, 1);
	cr_assert_str_eq(cmd->argv[0], "test");
	cr_assert_eq(cmd->chain_type, PIPE);

	cr_assert_str_eq(cmd->next->command, "echo");
	cr_assert_eq(cmd->next->argc, 1);
	cr_assert_str_eq(cmd->next->argv[0], "123123");
}

Test(command_parse, and_command_chain) {
	command* cmd = command_parse("echo test test1 &&cat 123123");

	cr_assert_str_eq(cmd->command, "echo");
	cr_assert_eq(cmd->argc, 2);
	cr_assert_str_eq(cmd->argv[0], "test");
	cr_assert_str_eq(cmd->argv[1], "test1");
	cr_assert_eq(cmd->chain_type, AND);

	cr_assert_str_eq(cmd->next->command, "cat");
	cr_assert_eq(cmd->next->argc, 1);
	cr_assert_str_eq(cmd->next->argv[0], "123123");
}

Test(command_parse, command_chain_with_escaping_character) {
	command* cmd = command_parse("cat test\\ test1 & echo 123123");

	cr_assert_str_eq(cmd->command, "cat");
	cr_assert_eq(cmd->argc, 1);
	cr_assert_str_eq(cmd->argv[0], "test test1");
	cr_assert_eq(cmd->chain_type, BACKGROUND);

	cr_assert_str_eq(cmd->next->command, "echo");
	cr_assert_eq(cmd->next->argc, 1);
	cr_assert_str_eq(cmd->next->argv[0], "123123");
}

Test(command_parse, command_stdout_redirection) {
	command* cmd = command_parse("echo \"123123\" => archive.txt");

	cr_assert_str_eq(cmd->command, "echo");
	cr_assert_eq(cmd->argc, 1);
	cr_assert_str_eq(cmd->argv[0], "123123");
	cr_assert_str_eq(cmd->stdout_file_redirection, "archive.txt");
}

Test(command_parse, command_stdin_redirection) {
	command* cmd = command_parse("echo \"123123\" <= archive.txt");

	cr_assert_str_eq(cmd->command, "echo");
	cr_assert_eq(cmd->argc, 1);
	cr_assert_str_eq(cmd->argv[0], "123123");
	cr_assert_str_eq(cmd->stdin_file_redirection, "archive.txt");
}

Test(command_parse, complex_command) {
	command* cmd = command_parse("echo \"123123 1\" <= archive.txt\\ \"teste\"|cat=>saida\".txt\"  ");

	cr_assert_str_eq(cmd->command, "echo");
	cr_assert_eq(cmd->argc, 1);
	cr_assert_str_eq(cmd->argv[0], "123123 1");
	cr_assert_str_eq(cmd->stdin_file_redirection, "archive.txt teste");
	cr_assert_eq(cmd->chain_type, PIPE);

	cr_assert_str_eq(cmd->next->command, "cat");
	cr_assert_eq(cmd->next->argc, 0);
	cr_assert_str_eq(cmd->next->stdout_file_redirection, "saida.txt");
}

Test(command_parse, echo_invalid_with_double_quotes_and_escaping_character, .init=redirect_stderr) {
	command_parse("echo out\" in \\\"out");

	cr_assert_stderr_eq_str(
		"Unexpected EOF while looking for matching \"\n",
		"Failed to parse invalid command escaping last double quote"
	);
}

Test(command_parse, echo_invalid_with_quotes_and_escaping_character, .init=redirect_stderr) {
	command_parse("\'echo test");

	cr_assert_stderr_eq_str(
		"Unexpected EOF while looking for matching \'\n",
		"Failed to parse invalid command missing last quote"
	);
}
