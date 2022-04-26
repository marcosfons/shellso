
#include <criterion/criterion.h>
#include <criterion/internal/assert.h>
#include <criterion/internal/test.h>
#include <criterion/redirect.h>
#include <stdio.h>
#include <string.h>

#include "../../src/shell/shell.c"




// Test(run_for_line, single_command, .init=cr_redirect_stdout) {
// 	run_for_line(strcpy(malloc(39), "wc -l tests/test_files/lorem_ipsum.txt"));
// 	cr_assert_stdout_eq_str("19 tests/test_files/lorem_ipsum.txt\n");
// }
//
// Test(run_for_line, piping_command) {
// 	// run_for_line(strcpy(malloc(45), "pwd"));
// 	run_for_line(strcpy(malloc(45), "cat tests/test_files/lorem_ipsum.txt | wc -l"));
// 	cr_assert_stdout_eq_str("19");
// }


// Test(str_realloc_escaping_character, sample) {
// 	char* dest = malloc(sizeof(char));
// 	dest[0] = '\0';
// 	int size = 1;
//
// 	str_realloc_escaping_character(dest, &size, "Testing\\\"", 7);
//
// 	cr_assert_str_eq(dest, "Testing\"");
// 	cr_assert_eq(size, 9);
// }
//
