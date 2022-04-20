
#include <criterion/criterion.h>
#include <criterion/internal/assert.h>
#include <criterion/internal/test.h>
#include <criterion/redirect.h>
#include <stdio.h>
#include <string.h>

#include "../../src/shell/shell.c"


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
