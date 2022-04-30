#ifndef COMMAND_HEADER
#define COMMAND_HEADER


#define PID_COMMAND_NOT_EXECUTED_YET -1
#define STATUS_COMMAND_NOT_EXECUTED_YET -1

/** Type of a chain between two commands */
typedef enum {
	NONE,				/**< Doesn't have a next command */
	PIPE,				/**< |  Sends output to the next command */
	OR,					/**< || Execute another command afterwards */
	BACKGROUND,	/**< &  Execute the program in background */
	AND,				/**< && Only executes if the last command was successful */
} command_chain_type;


/** @brief A structure to represent a command chain that will be executed */
typedef struct command {
	/// Infos
	char* command; 								 /**< The command */
	int argc; 										 /**< The number of arguments */
	char** argv; 									 /**< The arguments */
	struct command* next; 			 	 /**< The next command in the chain */
	command_chain_type chain_type; /**< The type of the command chain */
	char* stdin_file_redirection;	 /**< The file that will be the input file */
	char* stdout_file_redirection; /**< The file that the stdout will be redirected for */
	char* stderr_file_redirection; /**< The file that the stdout will be redirected for */

	/// Execution
	int pid;
	int status;
} command;


command* command_create();

void command_set_command(command* cmd, char* input);

void command_add_argument(command* cmd, char* argument);

void command_set_stdout_redirection(command* cmd, char* filepath);

void command_set_stdin_redirection(command* cmd, char* filepath);

/**
 * @brief Frees the given command
 * @param command The command that will be freed
 */
void command_free(command* command);

#endif
