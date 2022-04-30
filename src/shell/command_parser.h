#ifndef COMMAND_PARSER_HEADER
#define COMMAND_PARSER_HEADER

#include "command.h"

/**
 * @brief Create a new command
 * @param input The input string
 * @return The newly created command
 */
command* command_parse(char* input);

#endif
