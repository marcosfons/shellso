#include <criterion/criterion.h>
#include <criterion/internal/assert.h>
#include <criterion/internal/test.h>
#include <criterion/redirect.h>
#include <stdio.h>
#include <string.h>

#include "../../src/shell/alias.h"


#define ARBITRARY_ALIASSES_HASH_SIZE 57

/// @todo Missing tests for free

Test(alias, create_aliasses) {
	aliasses* aliasses = create_aliasses(ARBITRARY_ALIASSES_HASH_SIZE);

	cr_assert_not_null(aliasses);
	cr_assert_eq(aliasses->size, ARBITRARY_ALIASSES_HASH_SIZE);
	cr_assert_eq(aliasses->count, 0);
	cr_assert_not_null(aliasses->aliasses);
}

Test(alias, add_simple_alias) {
	aliasses* aliasses = create_aliasses(ARBITRARY_ALIASSES_HASH_SIZE);

	char* new_alias[] = { "ls", "--color=auto" };
	add_alias(aliasses, "ls", 2, new_alias);

	cr_assert_eq(aliasses->count, 1);
}

Test(alias, add_alias_and_find_it) {
	aliasses* aliasses = create_aliasses(ARBITRARY_ALIASSES_HASH_SIZE);

	char* new_alias[] = { "ls", "--color=auto" };
	add_alias(aliasses, "ls", 2, new_alias);

	alias* item = find_alias(aliasses, new_alias[0]);

	cr_assert_not_null(item);
	cr_assert_eq(item->argc, 2);
	cr_assert_eq(item->argv, new_alias);
	cr_assert_str_eq(item->argv[0], new_alias[0]);
	cr_assert_str_eq(item->argv[1], new_alias[1]);
}

Test(alias, search_non_added_alias) {
	aliasses* aliasses = create_aliasses(ARBITRARY_ALIASSES_HASH_SIZE);

	alias* item = find_alias(aliasses, "ls");

	cr_assert_null(item);
}

Test(alias, search_between_two_aliasses) {
	aliasses* aliasses = create_aliasses(ARBITRARY_ALIASSES_HASH_SIZE);

	char* ls[] = { "ls", "--color=auto" };
	add_alias(aliasses, "ls", 2, ls);
	char* grep[] = { "grep", "--color=auto" };
	add_alias(aliasses, "grep", 2, grep);

	alias* item = find_alias(aliasses, "grep");

	cr_assert_not_null(item);
	cr_assert_eq(item->argc, 2);
	cr_assert_eq(item->argv, grep);
}
