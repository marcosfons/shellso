#ifndef SHELL_HEADER
#define SHELL_HEADER


#include <stdio.h>

#define MAX_COMMAND_LENGTH 512
#define MAX_TOKEN_LENGTH 64


typedef struct {

} shell_t;

typedef struct {

} shell_builtin_command_t;

void run_interactive();
void run_for_file(FILE *fp);

void run_for_line(char* input);


#endif
