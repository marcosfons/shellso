#ifndef COMMAND_PARSER_HEADER
#define COMMAND_PARSER_HEADER


/** Type of a chain between two commands */
typedef enum {
	NONE,				/**< Doesn't have a next command */
	PIPE,				/**< |  Sends output to the next command */
	AND,				/**< && Only executes if the last command was successful */
	OR,					/**< || Execute another command afterwards */
	BACKGROUND	/**< &  Execute the program in background */
} command_chain_type;


/** @brief A structure to represent a command chain */
typedef struct command_t {
	char* command; 								 /**< The command */
	int argc; 										 /**< The number of arguments */
	char** argv; 									 /**< The arguments */
	struct command_t* next; 			 /**< The next command in the chain */
	command_chain_type chain_type; /**< The type of the command chain */
	char* stdin_file_redirection;	 /**< The file that will be the input file */
	char* stdout_file_redirection; /**< The file that the stdout will be redirected for */
	char* stderr_file_redirection; /**< The file that the stdout will be redirected for */
} command_t;


/**
 * @brief Create a new command
 * @param input The input string
 * @return The newly created command
 */
command_t* command_parse(char* input);

/**
 * @brief Frees the given command
 * @param command The command that will be freed
 */
void command_free(command_t* command);


#endif
