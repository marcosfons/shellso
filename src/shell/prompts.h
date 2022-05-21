#ifndef PROMPTS_HEADER
#define PROMPTS_HEADER


#include "shell.h"


/// @brief Simple shell prompt that prints the name of the user and the cwd
///
/// @param shell The running shell
void normal_shell_prompt(shell* shell);

void tp_shell_prompt(shell* shell);


#endif
