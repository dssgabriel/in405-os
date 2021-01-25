#ifndef __H_lexicon__
#define __H_lexicon__

// Defines a word
typedef struct entry {
	char *word;
	struct entry *next;
} entry;

// Defines a lexicon
typedef struct lexicon {
	char *name;
	int	mutable;
	int number;
	entry *first;
} lexicon;

// Initializes a lexicon data structure
// The lexicon must be freed using the lexc_finish() function
// @param 	 	name 		lexicon name
// @param 	 	mutable 	TRUE if the lexicon is mutable
// @return 		New lexicon
lexicon *lexiconInit(char *name, int mutable);

// Releases a lexicon data structure
// @param 		lex 		lexicon to release
void lexiconFree(lexicon *lex);

// Adds a word to a lexicon data structure
// @param		lex 		lexicon receiving the word
// @param		word 		Word to add
// @return 	 	0 on SUCCESS - 1 on FAILURE
int lexiconAdd(lexicon *lex, char *word);

// Removes a word from a lexicon data structure
// @param 		lex 		lexicon
// @param 		word 		Word to remove
// @return 		0 on SUCCESS - 1 on FAILURE
int lexiconRemove(lexicon *lex, char *word);

// Checks if a word is in a lexicon data structure
// @param 		lex 		lexicon
// @param 		word 		Word to check
// @return 		0 on SUCCESS - 1 on FAILURE
int lexiconCheck(lexicon *lex, char *word);

// Shows the entry of a lexicon
// @param 		lex 		lexicon
// @return 		0 on SUCCESS - 1 on FAILURE
int lexiconShow(lexicon *lex);

// Interprets a command and runs the corresponding function
// @param 		lex 		lexicon
// @param 		cmd 		Command
// @return 		0 on SUCCESS - 1 on FAILURE
int cmdInterpret(lexicon *lex, char *cmd);

#endif