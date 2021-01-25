#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexc_err.h"

#define ACTION_SIZE 16
#define WORD_SIZE   48

#define LOCK_OFF    1
#define LOCK_ON     0
#define LOCK_UNDEF -1

#define MODE_LEX 	1
#define MODE_DIC 	0
#define MODE_UNDEF -1

#define MAX_CMD 10


typedef struct word_s {
	char *w;
  	int len;
  	struct word_s *next;
} word_t;

typedef struct lexc {
  	char *name;
  	int lock;
  	int mode;
  	word_t *r;
  	unsigned long long nb_words;
} lexc_t;


lexc_t *lexc_init(const char *name, const int mutable, const int mode) {
	// Allocate memory for a lexc variable
	lexc_t *l = malloc(sizeof(lexc_t));
	
	if(l) {
    	// Calculate the length of the name string
    	int name_len = strlen(name);
      
	    // Allocate memory for the null terminated name string 
	    l->name = malloc(sizeof(char) * (name_len + 1));

	    // Copy the name string into the lexc name field
	    strncpy(l->name, name, name_len); 

	    // Make sure the string is null terminated
	    l->name[name_len] = 0;
	      
	    // Initialise 
	    l->r = NULL;
	    l->nb_words = 0;
	    l->lock = mutable;
	    l->mode = mode;
    }
  	return l;
}


void lexc_fini(lexc_t *lexc) {
	if(lexc) {

  		// Free name
    	if(lexc->name) {
			free(lexc->name);
			lexc->name =  NULL;
		}
    	lexc->lock = LOCK_UNDEF;

    	// Free linked list
    	if(lexc->r) {
	  		word_t *w_ptr      = lexc->r;
	  		word_t *prev_w_ptr = lexc->r;

	 		while(w_ptr) {
	      		// Save the current position (the one to be freed)
	      		prev_w_ptr = w_ptr;

	      		// Move to the next node
	      		w_ptr = w_ptr->next;
	      
	      		// Free the current node's word string
	      		if(prev_w_ptr->w)
					free(prev_w_ptr->w);

				// Free the current node
				free(prev_w_ptr);
	    	}
		lexc->r = NULL;
		}
    }
}


int lexc_add(lexc_t *lexc, const char *word) {
	if(lexc && word) {
      	if(lexc->lock == LOCK_OFF) {
	  		word_t *w_ptr = lexc->r;
	  		int word_len  = strlen(word);
	  		// If first word
	  		if(!w_ptr) {

	      		// Allocate a word
	      		lexc->r = malloc(sizeof(word_t));
	      		if(word_len >= WORD_SIZE)
	      			return ERR_WORD_SIZE_TOO_LARGE;

	     		// Allocate a null terminated word string
	      		lexc->r->w = malloc(sizeof(char) * (word_len + 1));
	      		strncpy(lexc->r->w, word, word_len);
	      		lexc->r->w[word_len] = 0;
	      		lexc->r->len = word_len;
	      		lexc->r->next = NULL;
	    	} 
	    	else if(lexc->mode == MODE_LEX) {
	      		int not_found = strncmp(w_ptr->w, word, word_len);

	      		// Reach last word
	  			while(not_found && w_ptr->next) {

		  			// found == 0 is the two strings are similar
		  			if(w_ptr->len == word_len)
		    			not_found = strncmp(w_ptr->w, word, word_len);
		  			if(not_found)
		   				w_ptr = w_ptr->next;
				}

				// Add at the end of the linked list
	     		if(not_found) {
		  			word_t *w_new = malloc(sizeof(word_t));
		  			w_new->w = malloc(sizeof(char) * (word_len + 1));
		  			strncpy(w_new->w, word, word_len);
		  			w_new->w[word_len] = 0;
		  			w_new->len = word_len;
		  			w_new->next = NULL;

		  			// Link to the previous last node
		  			w_ptr->next = w_new;
				} else
					return ERR_WORD_EXISTS;
	    	} 
	    	else if(lexc->mode == MODE_DIC) {
	    		int not_found = strncmp(w_ptr->w, word, word_len);

	    		// Find previous word



















	    	}
			lexc->nb_words++;
	  		return EXEC_SUCCESS;
		} else
			return ERR_LEXC_LOCK_ON;
    } else
    	return ERR_NULL_PARAMETER;
}


int lexc_remove(lexc_t *lexc, const char *word) {
 	if(lexc && word) {      
      	if(lexc->lock == LOCK_OFF) {
			word_t *w_ptr      = lexc->r;
			word_t *prev_w_ptr = lexc->r;

	  		// If list not empty
	  		if(w_ptr) {
	      		int word_len = strlen(word);
	      		int not_found = strncmp(w_ptr->w, word, word_len);
	      		while(not_found && w_ptr) {
		  			if(w_ptr->len == word_len)
		    			not_found = strncmp(w_ptr->w, word, word_len);
		  			if(not_found) {
		      			prev_w_ptr = w_ptr;
		      			w_ptr = w_ptr->next;
		    		}
				}
	      		if(not_found)
					return ERR_WORD_NOT_FOUND;
	      		else {
		  			if(prev_w_ptr != w_ptr) {
		  				// Link previous to the next
		    			prev_w_ptr->next = w_ptr->next;
		     			free(w_ptr->w);
		      			free(w_ptr);
		    		} else {
		    			// First node
						lexc->r = w_ptr->next;
		      			free(w_ptr->w);
		      			free(w_ptr);
			    	}
				}
				lexc->nb_words--;
	      		return EXEC_SUCCESS;
	    	} else
	    		return ERR_LIST_EMPTY;
	    } else
			return ERR_LEXC_LOCK_ON;
    } else
    	return ERR_NULL_PARAMETER;
}


int lexc_check(lexc_t *lexc, const char *word) {
  	if(lexc && word) {
    	word_t *w_ptr = lexc->r;
      	int word_len = strlen(word);

		if(w_ptr) {
	  		int not_found = strncmp(w_ptr->w, word, word_len);

		 	while(not_found && w_ptr) {
	      		if(w_ptr->len == word_len)
					not_found = strncmp(w_ptr->w, word, word_len);
				if(not_found)
					w_ptr = w_ptr->next;
	    	}
			if(not_found)
	    		return ERR_WORD_NOT_FOUND;
		} else
			return ERR_LIST_EMPTY;
      	return EXEC_SUCCESS;
    } else
    	return ERR_NULL_PARAMETER;
}


void lexc_print(lexc_t *lexc) {
	if(lexc) {
      	printf("\nLex [%s]\n", lexc->name);
      	printf("\tlock\t\t:\t %d (%s)\n", lexc->lock, 
      		(lexc->lock == LOCK_ON) ? "ON" : (lexc->lock == LOCK_OFF) ? "OFF" : "UDEF");
      	printf("\tmode\t\t:\t %d (%s)\n", lexc->mode, 
      		(lexc->mode == MODE_LEX) ? "LEX" : (lexc->mode == MODE_DIC) ? "DIC" : "UDEF");
      	printf("\twords count\t:\t %llu\n\n", lexc->nb_words);

      	word_t *w_ptr = lexc->r;
      	unsigned long long counter = 0;

	    while(w_ptr) {
	  		printf("[%5llu]\t\t%s\t\t%d\n", counter, w_ptr->w, w_ptr->len);
	  		counter++;
	  		w_ptr = w_ptr->next;
		}

		printf("\n");
    }
}


// New lexicon		(n) *
// Select lexicon 	(s) *
// Lexicon state	(l)
// Lexicon mode		(m) *
// Add word  		(a)
// Remove word  	(r)
// Check word  		(c)
// Print lexicon	(p)
// Help    			(h)
// Quit      		(q)
int cmd_interpret(lexc_t *lexc, const char *cmd) {
  	int ret = 0;
  	char tmp_word[WORD_SIZE + 1];

	// if(cmd[0] == 'n') {
	// 	strcpy(tmp_word, cmd + 2);

	// 	int word_len = strlen(tmp_word);
	// 	tmp_word[word_len - 1] = 0;

	// 	ret = lexc_init(tmp_word, 1 , 1);
	// }

	// else if(cmd[0] == 's') {
	// 	strcpy(tmp_word, cmd + 2);

 	//     	int word_len = strlen(tmp_word);
 	//      tmp_word[word_len - 1] = 0;

 	//     	ret = ;
	// } 

	// else if(cmd[0] == 'm') {
	// 	strcpy(tmp_word, cmd + 2);

	// 	int word_len = strlen(tmp_word);
	// 	tmp_word[word_len - 1] = 0;

	// 	if(tmp_word[0] == 'L' && tmp_word[1] == 'E' && tmp_word[2] == 'X') {
	// 		lexc->mode = MODE_LEX;
		
	// 	else if(tmp_word[0] == 'D' && tmp_word[1] == 'I' && tmp_word[2] == 'C')
	// 		lexc->mode == MODE_DIC;

	// 	else
	// 		ret = ERR_BAD_MODE_VALUE;
	// 	} else
	// 		ret = ERR_BAD_MODE_VALUE;
	// }

	/*else*/ if(cmd[0] == 'l') {
	    strcpy(tmp_word, cmd + 2);
	    
	    int word_len = strlen(tmp_word);
	    tmp_word[word_len - 1] = 0;
	    
	    if(tmp_word[0] == 'O') {
			if(tmp_word[1] == 'N')
		  		lexc->lock = LOCK_ON;
			else
		  	if(tmp_word[1] == 'F' && tmp_word[2] == 'F')
		    	lexc->lock = LOCK_OFF;

		  	else
		    	ret = ERR_BAD_LOCK_VALUE;
	    } else
	      	ret = ERR_BAD_LOCK_VALUE;
	}

	else if(cmd[0] == 'd')
		lexc_fini(lexc);

  	else if(cmd[0] == 'a') {
    	strcpy(tmp_word, cmd + 2);

      	int word_len = strlen(tmp_word);
      	tmp_word[word_len - 1] = 0;

      	ret = lexc_add(lexc, tmp_word);
    } 

   	else if (cmd[0] == 'r') {
  		strcpy(tmp_word, cmd + 2);

		int word_len = strlen(tmp_word);
		tmp_word[word_len - 1] = 0;

		ret = lexc_remove(lexc, tmp_word);
    } 

    else if (cmd[0] == 'c') {
		strcpy(tmp_word, cmd + 2);

		int word_len = strlen(tmp_word);
		tmp_word[word_len - 1] = 0;

		ret = lexc_check(lexc, tmp_word);

		if(!ret)
	    	printf("\t\t '%s' found in lexicon\n", tmp_word);
  	} 

	else if(cmd[0] == 'p')
	    lexc_print(lexc);

	else if(cmd[0] == 'h') {
		printf("\n\tAvailable commands:\n\n");
		// printf("\t\tn NAME       : create a new lexicon NAME\n");
		// printf("\t\ts NAME       : select the lexicon NAME\n");
		// printf("\t\tm LEX or DIC : set lexicon entry mode\n");
		printf("\t\tl ON or OFF  : set lexicon access mode\n");
		// printf("\t\td NAME 		 : delete a lexicon")
		printf("\t\ta WORD       : add a word to the lexicon\n");
		printf("\t\tr WORD       : remove a word from the lexicon\n");
		printf("\t\tc WORD       : check if a word is in the lexicon\n");
		printf("\t\tp            : print the lexicon\n");
		printf("\t\th            : print this help\n");
		printf("\t\tq            : exit\n\n");
	} 

	else if(cmd[0] == 'q')
		return printf("\n\tBye o/\n\n"), 0;

	else 
		ret = ERR_UNKNOWN_COMMAND;
 	lexc_error(ret);
  	
  	return 1;
}
