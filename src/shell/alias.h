#ifndef ALIAS_HEADER
#define ALIAS_HEADER


/** @brief Only one alias entry **/
typedef struct alias {
	char* key;   /**< The key associated with this alias, like "ls" */
	int argc;    /**< The argument count of the alias */
	char** argv; /**< The argument list of the alias */
} alias;

/** @brief Hash table of aliasses used in the shell */
typedef struct aliasses {
	alias** aliasses; /**< A list of builtin commands */
	int size;         /**< The entire size of the list of builtin commands */
	int count;        /**< How many items are stored in the list */
} aliasses;


/// @brief Create the hash map of aliasses
///
/// @param size The initial size of the hash map
///
/// @return Returns a pointer to the newly created aliasses hash map
aliasses* create_aliasses(int size);

/// @brief Add an alias to the hash map
///
/// @param aliasses A pointer to the aliasses hash map
/// @param key The key that identifies the alias
/// @param argc The argument count of the alias
/// @param argv The list of arguments starting with the command
void add_alias(aliasses* aliasses, char* key, int argc, char** argv);

/// @brief Calculate the hash value of the input string
/// 
/// @param aliasses The aliasses hash map
/// @param str The alias input string
///
/// @return The hash generated, limited by the aliasses size
unsigned long alias_hash_function(aliasses* aliasses, char* str);

/// @brief Find an alias in the hash map by a string
/// 
/// @param aliasses The aliasses hash map
/// @param alias The alias string
///
/// @return Returns an alias pointer or NULL if it was not find
alias* find_alias(aliasses* aliasses, char* alias);

/// @brief Frees a single alias
/// 
/// @param alias The alias that will be freed
void free_alias(alias* alias);

/// @brief Frees the aliasses hash map
///
/// @param aliasses The aliasses hash map
void free_aliasses(aliasses* aliasses);


#endif
