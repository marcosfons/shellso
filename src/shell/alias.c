#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "alias.h"


aliasses* create_aliasses(int size) {
	aliasses* new_aliasses = malloc(sizeof(aliasses));

	new_aliasses->size = size;
	new_aliasses->count = 0;
	new_aliasses->aliasses = calloc(new_aliasses->size, sizeof(alias*));

	return new_aliasses;
}

void add_alias(aliasses* aliasses, char* key, int argc, char** argv) {
	assert(argc > 0 && argv[0] != NULL);
	
	unsigned long hash = alias_hash_function(aliasses, argv[0]);
	if (aliasses->aliasses[hash] != NULL) {
		printf("Aliasses hash collision\n");
		return;
	}

	alias* new_alias = malloc(sizeof(alias));
	new_alias->key = key;
	new_alias->argc = argc;
	new_alias->argv = argv;

	aliasses->aliasses[hash] = new_alias;
	aliasses->count += 1;
}

unsigned long alias_hash_function(aliasses* aliasses, char* str) {
	unsigned long i = 0;
	for (int j=0; str[j]; j++) {
		i += str[j];
	}
	return i % aliasses->size;
}

alias* find_alias(aliasses* aliasses, char* key) {
	unsigned long hash = alias_hash_function(aliasses, key);
	
	alias* item = aliasses->aliasses[hash];

	if (item != NULL && strcmp(item->key, key) == 0) {
		return item;
	}

	return NULL;
}

void free_alias(alias* alias) {
	for (int i = 0; i < alias->argc; i++) {
		free(alias->argv[i]);
	}

	free(alias->argv);
	free(alias);
}

void free_aliasses(aliasses* aliasses) {
	for (int i = 0; i < aliasses->size; i++) {
		if (aliasses->aliasses[i] != NULL) {
			free_alias(aliasses->aliasses[i]);
		}
	}

	free(aliasses->aliasses);
	free(aliasses);
}
