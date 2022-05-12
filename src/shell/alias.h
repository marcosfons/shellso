#ifndef ALIAS_HEADER
#define ALIAS_HEADER


/** @brief Only one alias entry **/
typedef struct alias {
	char* alias;
	int value_count;
	char** value;
} alias;

/** @brief Hash table of aliasses used in the shell */
typedef struct aliasses {
	alias* aliasses;
	int size;
	int count;
} aliasses;


aliasses* create_aliasses();

void add_alias(aliasses* aliasses, char* alias, char** value);

// unsigned long hash_function(aliasses* aliasses, char* str);

char** find_alias(aliasses* aliasses, char* alias);

void free_alias(alias* alias);

void free_aliasses(aliasses* aliasses);

#endif
