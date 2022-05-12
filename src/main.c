#include <stdio.h>

#include "shell/shell.h"


int main(int argc, char** argv) {
	shell* shell = create_shell();
	run_interactive(shell);

	free_shell(shell);

	return 0;
}
