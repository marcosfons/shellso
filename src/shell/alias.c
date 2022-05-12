#include <stdio.h>
#include <stdlib.h>


#include "alias.h"


aliasses* create_aliasses() {
	aliasses* new_aliasses = malloc(sizeof(aliasses));

	new_aliasses->size = 23;
	new_aliasses->count = 0;

	return new_aliasses;
}

void add_alias(aliasses* aliasses, char* alias, char** value);

// unsigned long hash_function(aliasses* aliasses, char* str) {
// 	unsigned long i = 0;
// 	for (int j=0; str[j]; j++) {
// 		i += str[j];
// 	}
// 	return i % aliasses->size;
// }

// char** find_alias(aliasses* aliasses, char* alias) {
// 	
// }

void free_alias(alias* alias) {
	free(alias->alias);

	for (int i = 0; i < alias->value_count; i++) {
		free(alias->value[i]);
	}

	free(alias->value);
	free(alias);
}

void free_aliasses(aliasses* aliasses) {
	for (int i = 0; i < aliasses->count; i++) {
		free_alias(aliasses->aliasses + i);
	}

	free(aliasses);
}
