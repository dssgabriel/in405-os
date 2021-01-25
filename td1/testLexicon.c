#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexicon.h"

#define PRINT_ON_FAILURE(a)  { fprintf(stderr, "FAIL -- %s\n", a); return 1; }

#define CMD_BUFFER	64


// Unit test - function lexiconAdd()
// @return 		0 on SUCCESS - 1 on FAILURE
int testUnitAdd() {
	lexicon *L = lexiconInit("test", 1);
	lexicon *M = lexiconInit("test-im", 0);

	assert(L);
	assert(M);

	// test on bad lexicons
	if(!lexiconAdd(NULL, "test"))
		PRINT_ON_FAILURE("Add to 'NULL' lexicon must fail");
	if(!lexiconAdd(M, "test"))
		PRINT_ON_FAILURE("Add to immutable lexicon must fail");

	// test with a bad word
	if(!lexiconAdd(L, NULL))
		PRINT_ON_FAILURE("Add a 'NULL' word must fail");

	// test with a good word
	if(lexiconAdd(L, "test"))
		PRINT_ON_FAILURE("Add to mutable lexicon must succeed");

	// test with an existent word
	if(!lexiconAdd(L, "test"))
		PRINT_ON_FAILURE("Duplicated add must fail");

	lexiconFree(L);
	lexiconFree(M);

	return 0;
}


// Unit test - function lexiconRemove()
// @return 	0 on SUCCES - 1 on FAILURE
int testUnitRemove() {
	lexicon *L = lexiconInit("test", 1);
	lexicon *M = lexiconInit("test-im", 0);

	assert(L);
	assert(M);
	assert(!lexiconAdd(L, "test"));

	// test on bad lexicons
	if(!lexiconRemove(NULL, "test"))
		PRINT_ON_FAILURE("Remove from 'NULL' lexicon must fail");
	if(!lexiconRemove(M, "test"))
		PRINT_ON_FAILURE("Remove from immutable lexicon must fail");

	// test with a bad word
	if(!lexiconRemove(L, NULL))
		PRINT_ON_FAILURE("Remove a 'NULL' word must fail");

	// test with a good word
	if(lexiconRemove(L, "test"))
		PRINT_ON_FAILURE("Remove from mutable lexicon must succeed");

	// test with a non-existent word
	if(!lexiconRemove(L, "test"))
		PRINT_ON_FAILURE("Remove a non-existent word must fail");

	lexiconFree(L);
	lexiconFree(M);

	return 0;
}


// Unit test - function lexiconCheck() 
// @return 	0 on SUCCES - 1 on FAILURE
int testUnitCheck() {
	lexicon *L = lexiconInit("test", 1);

	assert(L);
	assert(!lexiconAdd(L, "test"));

	// test on bad lexicon
	if(!lexiconCheck(NULL, "test"))
		PRINT_ON_FAILURE("Check from 'NULL' lexicon must fail");

	// test with a bad word
	if(!lexiconCheck(L, NULL))
		PRINT_ON_FAILURE("Check a 'NULL' word must fail");

	// test with a good word
	if(lexiconCheck(L, "test"))
		PRINT_ON_FAILURE("Check an existent word must succeed");

	// test with a non-existent word
	if(!lexiconCheck(L, "no-test"))
		PRINT_ON_FAILURE("Check a non-existent word must fail");

	lexiconFree(L);

	return 0;
}

// Integration test - Add the alphabet letters in the lexicon
// then remove them
// @return 		0 on SUCCESS - 1 on FAILURE
int testIntegrationScenarioAlphabet() {
	lexicon *L;
	char c[2] = "a\0";

	L = lexiconInit("alpha", 1);
	if(!L)
		return 1;

	for(;c[0] <= 'z'; ++c[0])
		if(lexiconAdd(L, c))
			return 1;

	--c[0];
	for(;c[0] >= 'a'; --c[0])
		if(lexiconRemove(L, c))
			return 1;

	lexiconFree(L);

	return 0;
}


// Integration test - Add to and remove from the lexicon
// the same word
// @return 	0 on SUCCESS - 1 on FAILURE
int testIntegrationScenarioDoAndRedo() {
	lexicon *L;
	L = lexiconInit("test", 1);
	if(!L)
		return 1;

	for(int i = 0; i < 10; i++) {
		if(lexiconAdd(L, "do"))
			return 1;
		if(lexiconRemove(L, "do"))
			return 1;
	}

	lexiconFree(L);

	return 0;
}


// Executes the unit and integration tests
// Uncomment them once their implementation are done
void execute_test() {
	printf("[U-TEST] add: %s\n", testUnitAdd() ? "FAILURE" : "SUCCESS");
	printf("[U-TEST] remove: %s\n", testUnitRemove() ? "FAILURE" : "SUCCESS");
	printf("[U-TEST] check: %s\n", testUnitCheck() ? "FAILURE" : "SUCCESS");

	printf("[I-TEST] scenario-alphabet: %s\n",
			testIntegrationScenarioAlphabet() ? "FAILURE" : "SUCCESS");
	printf("[I-TEST] scenario-do-and-redo: %s\n",
			testIntegrationScenarioDoAndRedo() ? "FAILURE" : "SUCCESS");
}


// Executes the command interpreter until the command 'quit' is entered.
void execute() {
	char buffer[CMD_BUFFER];
	lexicon *L;

	L = lexiconInit("test", 1);

	do {
		printf("Command to run?\n");
		scanf("%s", buffer);
		if(strcmp(buffer, "quit") == 0) {
			lexiconFree(L);
			break;
		}
	} while (!cmdInterpret(L, buffer)); 
}


// Main function
// You can define the TESTlexicon variable by compiling your program
// with the following command, using the -D gcc option:
// $ gcc -DTESTlexicon testlexicon.c lexicon.c
int main() {
#ifdef TEST_LEXC
	execute_test();
#else
	execute();
#endif
	return 0;
}