#ifndef COMMAND_PARSER_HEADER
#define COMMAND_PARSER_HEADER


#define MAX_COMMAND_LENGTH 512
#define MAX_TOKEN_LENGTH 64


/*! Type of a chain between two commands */
typedef enum {
	NONE,				/**< Doesn't have a next command */
	PIPE,				/**< |  Sends output to the next command */
	AND,				/**< && Only executes if the last command was successful */
	OR,					/**< || Execute another command afterwards */
	BACKGROUND	/**< &  Execute the program in background */
} command_chain_type;


/** Holds the data of a command chain
 * 
 */
typedef struct command_t {
	char* command;
	int argc;
	char** argv;
	struct command_t* next;
	command_chain_type chain_type;
} command_t;

/** Parses the given input creating a new command_t */
command_t* command_parse(char* input);

/** Free a command */
void command_free(command_t* command);


#endif
