#ifndef ALIAS_HEADER
#define ALIAS_HEADER


/** @brief Only one alias entry **/
typedef struct alias {
	char* key;
	int argc;
	char** argv;
} alias;

/** @brief Hash table of aliasses used in the shell */
typedef struct aliasses {
	alias** aliasses;
	int size;
	int count;
} aliasses;


aliasses* create_aliasses(int size);

void add_alias(aliasses* aliasses, char* key, int argc, char** argv);

unsigned long alias_hash_function(aliasses* aliasses, char* str);

alias* find_alias(aliasses* aliasses, char* alias);

void free_alias(alias* alias);

void free_aliasses(aliasses* aliasses);

#endif
