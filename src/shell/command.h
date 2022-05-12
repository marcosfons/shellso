#ifndef COMMAND_HEADER
#define COMMAND_HEADER


#define PID_COMMAND_NOT_EXECUTED_YET -1
#define STATUS_COMMAND_NOT_EXECUTED_YET -1

/** Type of a chain between two commands */
typedef enum {
	NONE,       /**< Doesn't have a next command */
	PIPE,       /**< |  Sends output to the next command */
	OR,         /**< || Execute another command afterwards */
	BACKGROUND, /**< &  Execute the program in background */
	AND,        /**< && Only executes if the last command was successful */
} command_chain_type;


/** @brief A structure to represent a command chain that will be executed */
typedef struct command command;
typedef struct command {
	/// Infos
	int argc;                      /**< The number of arguments */
	char** argv;                   /**< The arguments. The first argument is the program */
	command* next;                 /**< The next command in the chain */
	command_chain_type chain_type; /**< The type of the command chain */
	char* stdin_file_redirection;  /**< The file that will be the input file */
	char* stdout_file_redirection; /**< The file that the stdout will be redirected for */
	char* stderr_file_redirection; /**< The file that the stdout will be redirected for */

	/// Execution
	int pid;
	int status;
} command;


/// @brief Creates one command setting default values
/// @return Returns a pointer to the newly created command
command* command_create();

/// @brief Set the command itself
///
/// @param cmd Of type command, containing command, arguments and other things
/// @param command Is the only the command part
void command_set_command(command* cmd, char* command);

/// @brief Adds one argument to the command
///
/// @param cmd Of type command, containing command, arguments and other things
/// @param argument The argument that will be added to the command
void command_add_argument(command* cmd, char* argument);

/// @brief Sets the redirection file of stdout
///
/// @param cmd Of type command, containing command, arguments and other things
/// @param filepath Filepath of the target stdout file
void command_set_stdout_redirection(command* cmd, char* filepath);

/// @brief Sets the redirection file of stdin
///
/// @param cmd Of type command, containing command, arguments and other things
/// @param filepath Filepath of the target stdin file
void command_set_stdin_redirection(command* cmd, char* filepath);

/**
 * @brief Frees the given command
 * @param command The command that will be freed
 */
void command_free(command* command);

#endif
