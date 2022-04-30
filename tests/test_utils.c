#include <criterion/redirect.h>

#include "test_utils.h"


void redirect_stderr(void) {
	cr_redirect_stderr();
}

void redirect_all_std(void) {
	cr_redirect_stdout();
	cr_redirect_stderr();
}
