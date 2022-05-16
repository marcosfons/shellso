#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <argp.h>

#include "shell/shell.h"


typedef struct arguments {
	enum { FROM_FILE, INTERACTIVE, FROM_STRING } mode;
	char* input_file;
	char* input_command;
} arguments;

const char *argp_program_version = "shellso";
static char doc[] = "A simple shell written in C";
static char args_doc[] = "[FILENAME]...";
static struct argp_option options[] = { 
	{ "from-file",   'f', "input",  0, "Run commands from the given file.",          1 },
	{ "interactive", 'i', 0,        0, "Run commands interactively. Default",        2 },
	{ "from-string", 'c', "string", 0, "Run commands from the given command string", 2 },
	{ 0, 0, 0, 0, 0, 0 } 
};

static int parse_opt(int key, char *arg, struct argp_state *state) {
	arguments *args = state->input;
		
	switch (key) {
		case 'i':
			args->mode = INTERACTIVE;
		case 'f': 
			args->mode = FROM_FILE; 
			args->input_file = arg; 
			break;
		case 'c': 
			args->mode = FROM_STRING;
			args->input_command = arg;
			break;
		case ARGP_KEY_ARG: 
			argp_usage(state);
			break;
		default: 
			return ARGP_ERR_UNKNOWN;
	}

	return 0;
}

static struct argp argp = { options, parse_opt, args_doc, doc };

int main(int argc, char** argv) {
	arguments args;
	args.mode = INTERACTIVE;

	argp_parse(&argp, argc, argv, 0, 0, &args);
	
	shell* shell = create_shell();

	if (args.mode == INTERACTIVE) {
		run_interactive(shell);
	} else if (args.mode == FROM_STRING) {
		run_from_string(shell, args.input_command);
	} else if (args.mode == FROM_FILE) {
		// run_from_file(shell, args.input_file);
	}

	free_shell(shell);

	return 0;
}
