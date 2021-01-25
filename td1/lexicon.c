#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexicon.h"

#define ACTION_SIZE 16
#define WORD_SIZE   48

#define TRUE 	0
#define FALSE 	1



// Initializes a lexicon data structure
// The lexicon must be freed using the lexc_finish() function
// @param 	 	name 		lexicon name
// @param 	 	mutable 	TRUE if the lexicon is mutable
// @return 		New lexicon
lexicon *lexiconInit() {
	lexicon *lex = malloc(sizeof(*lex));

	char *name;
	printf("Lexicon name?\n");
	scanf("%s", name);
	lex->name = name;

	int mutable;
	char *tmp;
	printf("Lexicon mutable? [Y/n]\n");
	scanf("%s", tmp);
	if(strcmp("Y", tmp))
		lex->mutable = 1;
	else
		lex->mutable = 0;

	lex->number = 0;
	lex->first = NULL;

    return lex;
}


// Releases a lexicon data structure
// @param 		lex 		lexicon to release
void lexiconFree(lexicon *lex) {
	entry *e;
	e = lex->first;

	while(e) {
		entry *tmp = e;
		free(tmp);
		e = e->next;
	}
	free(lex);
}


// Adds a word to a lexicon data structure
// @param		lex 		lexicon receiving the word
// @param		word 		Word to add
// @return 	 	0 on SUCCESS - 1 on FAILURE
int lexiconAdd(lexicon *lex, char *word) {
	if(!lex) {
		printf("ERROR: lexicon is NULL.\n");
		return 1;
	}
	if(lex->mutable == TRUE) {
		printf("ERROR: lexicon is immutable.\n");
		return 1;
	}
	if(!word) {
		printf("ERROR: word is NULL.\n");
		return 1;
	}
	if(lexiconCheck(lex, word)){
		printf("ERROR: word is already in lexicon.\n");
	}

	if(!lex->first) {
		entry *new = malloc(sizeof(*new));
		new->word = word;
		new->next = NULL;
		lex->first = new;
		lex->number += 1;
		printf("Word added.\n");
		return 0;
	}

	entry *e = malloc(sizeof(*e));
	e = lex->first;
	while(e)
		e = e->next;
	entry *new = malloc(sizeof(*new));
	new->word = word;
	new->next = NULL;
	e = new;
	lex->n++;
	printf("Word added.\n");
    return 0;
}


// Removes a word from a lexicon data structure
// @param 		lex 		lexicon
// @param 		word 		Word to remove
// @return 		0 on SUCCESS - 1 on FAILURE
int lexiconRemove(lexicon *lex, char *word) {
	if(!lex) {
		printf("ERROR: lexicon is NULL.\n");
		return 1;
	}
	if(!lex->first) {
		printf("ERROR: lexicon is initialized but empty.\n");
		return 1;
	}
	if(lex->mutable == TRUE) {
		printf("ERROR: lexicon is immutable.\n");
		return 1;
	}
	if(!word) {
		printf("ERROR: word is NULL.\n");
		return 1;
	}

	entry *e = malloc(sizeof(*e));
	e = lex->first;
	if(!e->next) {
		free(e);
		lex->n--;
		printf("Word removed.\n");
		return 0;
	}
	while(!strcmp(word, e->next->word) == 0) {
		e = e->next;
		if(!e) {
			printf("ERROR: word to remove is not in lexicon.\n");
			return 1;
		}
	}
	entry *prev, *next;
	prev = malloc(sizeof(entry));
	prev = e;
	next = malloc(sizeof(entry));
	next = e->next->next;
	free(e->next);
	prev->next = next;
	lex->n--;
	printf("Word removed.\n");
	return 0;
}


// Checks if a word is in a lexicon data structure
// @param 		lex 		lexicon
// @param 		word 		Word to check
// @return 		0 on SUCCESS - 1 on FAILURE
int lexiconCheck(lexicon *lex, char *word) {
	if(!lex) {
		printf("ERROR: lexicon is NULL.\n");
		return 1;
	}
	if(!word) {
		printf("ERROR: word is NULL.\n");
		return 1;
	}

	entry *e = malloc(sizeof(*e));
	e = lex->first;
	while(e) {
		if(strcmp(word, e->word) == 0) {
			printf("Word is in lexicon.\n");
			return 0;
		}
		e = e->next;
	}
	printf("Word is not in lexicon.\n");
    return 1;
}


// Shows the entry of a lexicon
// @param 		lex 		lexicon
// @return 		0 on SUCCESS - 1 on FAILURE
int lexiconShow(lexicon *lex) {
	if(!lex) {
		printf("ERROR: lexicon is NULL.\n");
		return 1;
	}
	// printf("%s %d\n", lex->name, lex->mutable);
	// printf("%s\n", lex->first->word);
	entry *e = malloc(sizeof(*e));
	e = lex->first;
	if(!e) {
		printf("ERROR: lexicon is initialized but empty.\n");
	}
	while(e) {
		printf("%s\n", e->word);
		printf("%d\n", lex->n);
		e = e->next;
	}
	return 0;
}


// Interprets a command and runs the corresponding function
// @param 		lex 		lexicon
// @param 		cmd 		Command
// @return 		0 on SUCCESS - 1 on FAILURE
int cmdInterpret(lexicon *lex, char *cmd) {
	if(!lex) {
		printf("ERROR: lexicon is NULL.\n");
		return 1;
	}

	char tmp[WORD_SIZE];
	if(strcmp(cmd, "add") == 0) {
		printf("Word to add?\n");
		scanf("%s", tmp);
		lexiconAdd(lex, tmp);
		return 0;
	}
	if(strcmp(cmd, "remove") == 0) {
		printf("Word to remove?\n");
		scanf("%s", tmp);
		lexiconRemove(lex, tmp);
		return 0;
	}
	if(strcmp(cmd, "check") == 0) {
		printf("Word to check?\n");
		scanf("%s", tmp);
		lexiconCheck(lex, tmp);
		return 0;
	}
	if(strcmp(cmd, "show") == 0) {
		lexiconShow(lex);
		return 0;
	} 
	else {
		printf("ERROR: command is not valid.\n");
		return 0;
	}
}