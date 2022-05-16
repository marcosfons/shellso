#include <criterion/criterion.h>
#include <criterion/internal/assert.h>
#include <criterion/internal/test.h>
#include <criterion/redirect.h>
#include <stdio.h>
#include <string.h>

#include "../../src/shell/shell.h"
#include "../../src/shell/builtin_command.c"

#define ARBITRARY_SIZE 103

#define HELP_BUILTIN_COMMAND_RETURN_VALUE 5

int empty_builtin_command_function(shell* shell, int argc, char** argv) {
	return 0;
}

int help_builtin_command_function(shell* shell, int argc, char** argv) {
	return HELP_BUILTIN_COMMAND_RETURN_VALUE;
}

Test(builtin_commands, create_shell_builtin_commands) {
	shell_builtin_commands* builtin = create_shell_builtin_commands(ARBITRARY_SIZE);

	cr_assert_not_null(builtin);
	cr_assert_eq(builtin->size, ARBITRARY_SIZE);
	cr_assert_eq(builtin->count, 0);
}

Test(builtin_commands, create_shell_builtin_commands_and_add_simple_commands) {
	shell_builtin_commands* builtin = create_shell_builtin_commands(ARBITRARY_SIZE);
	
	add_builtin_command(builtin, "cd", empty_builtin_command_function);
	add_builtin_command(builtin, "fg", empty_builtin_command_function);
	add_builtin_command(builtin, "jobs", empty_builtin_command_function);
	add_builtin_command(builtin, "help", empty_builtin_command_function);
	add_builtin_command(builtin, "time", empty_builtin_command_function);
	
	cr_assert_eq(builtin->size, ARBITRARY_SIZE);
	cr_assert_eq(builtin->count, 5);
}

Test(builtin_commands, create_shell_builtin_commands_add_simple_commands_and_find_help) {
	shell_builtin_commands* builtin = create_shell_builtin_commands(ARBITRARY_SIZE);
	
	add_builtin_command(builtin, "cd", empty_builtin_command_function);
	add_builtin_command(builtin, "fg", empty_builtin_command_function);
	add_builtin_command(builtin, "jobs", empty_builtin_command_function);
	add_builtin_command(builtin, "help", help_builtin_command_function);
	add_builtin_command(builtin, "time", empty_builtin_command_function);
	
	builtin_command_function function = find_builtin_command(builtin, "help");
	cr_assert_not_null(function);
	
	int result = function(NULL, 0, NULL);

	cr_assert_eq(result, HELP_BUILTIN_COMMAND_RETURN_VALUE);
}

// Test only if it gets any error
Test(builtin_commands, free_shell_builtin_commands) {
	shell_builtin_commands* builtin = create_shell_builtin_commands(ARBITRARY_SIZE);
	
	add_builtin_command(builtin, "cd", empty_builtin_command_function);
	add_builtin_command(builtin, "fg", empty_builtin_command_function);
	add_builtin_command(builtin, "jobs", empty_builtin_command_function);
	add_builtin_command(builtin, "help", help_builtin_command_function);
	add_builtin_command(builtin, "time", empty_builtin_command_function);

	free_shell_builtin_commands(builtin);
}
