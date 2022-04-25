#ifndef SHELL_HEADER
#define SHELL_HEADER


#include <stdio.h>

#include "command_parser.h"
#include "background_jobs.h"


// typedef struct {
// 	command command;
// 	bool running;
// 	int pid;
// } shell_state;
//
typedef struct {
	background_jobs* jobs;
} shell;
//
// typedef struct {
//
// } shell_builtin_command;
//


void run_command(command* cmd, int in_fd);
void run_interactive();
void run_for_file(FILE *fp);

void run_for_line(char* input);




#endif
