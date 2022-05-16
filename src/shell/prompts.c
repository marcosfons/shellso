#include <limits.h>
#include <stdlib.h>
#include <unistd.h>
#include <pwd.h>

#include "prompts.h"


void normal_shell_prompt(shell* shell) {
	size_t length = PATH_MAX;
	char* cwd = malloc(length);

	struct passwd* passwd = getpwuid(getuid());

	char* username = passwd != NULL ? passwd->pw_name : "user";

	if (getcwd(cwd, length) == NULL) {
		perror("cwd");
		printf("%s$ ", username);
	} else {
		printf("\033[03;32m%s:\033[03;34m%s\033[0m$ ", username, cwd);
	}

	free(cwd);
}

void tp_shell_prompt(shell* shell) {
	printf("Qual o seu comando? ");
}

