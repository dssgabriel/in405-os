#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include "se_fichier.h"

#define BUFFER_SIZE 16

/******************************************************************************/
/***** SYSTEMES D'EXPLOITATION ************************************************/
/***** TD8 - COMMUNICATION 2 **************************************************/
/***** EXERCICE 8.1 - LE MILLION **********************************************/
/******************************************************************************/
/***** @author DOS SANTOS Gabriel 21807297 TD4 ********************************/
/******************************************************************************/

/******************************************************************************/
/* "int ret;" is used to check if functions and memory allocations were done **/
/* correctly throughout the whole program thanks to assert(). *****************/
/******************************************************************************/


// Server settings data structure
typedef struct server {
	int pid;
	int numbers_per_ticket;
	int *winning_ticket;
	int **earnings;
} SERVER_SETTINGS;

// Ticket data structure
typedef struct ticket {
	int pid;
	int npt;
	int *numbers;
} TICKET;


/******************************************************************************/
/***** SERVER *****************************************************************/
/******************************************************************************/

// Show server settings in console
// parameter 	S 		SERVER_SETTINGS data structure
void show_SERVER_SETTINGS(SERVER_SETTINGS S) 
{
	printf("Server n°%d is ready.\n", S.pid);

	printf("Number of digits on tickets: \t%d\n", S.numbers_per_ticket);

	printf("Winning combination: \t\t");
	for (int i = 0; i < S.numbers_per_ticket; ++i)
		printf("%d ", S.winning_ticket[i]);

	printf("\nPossible earnings: ");
	printf("\t\t%d numbers right: \t$%d\n", S.earnings[0][0], S.earnings[1][0]);
	for (int i = 1; i < S.numbers_per_ticket; ++i)
	{
		if (S.earnings[0][i] != 1)
			printf("\t\t\t\t%d numbers right: \t$%d\n", S.earnings[0][i], S.earnings[1][i]);
		else
			printf("\t\t\t\t%d number  right: \t$%d\n", S.earnings[0][i], S.earnings[1][i]);
	}
}


//Free memory of server's SERVER_SETTINGS
// parameter 	S 				SERVER_SETTINGS data structure to free
void free_server(SERVER_SETTINGS S)
{
	free(S.winning_ticket);
	for (int i = 0; i < S.numbers_per_ticket; ++i)
		free(S.earnings[i]);
	free(S.earnings);
}


//Server writes results from processing to FIFO
// parameter 	S 				SERVER_SETTINGS data structure that
//								writes to FIFO
// parameter 	ticket_pid		PID of client's TICKET
// parameter 	right_numbers	How many numbers were right in 
//								client's TICKET
int write_server(SERVER_SETTINGS S, int ticket_pid, int right_numbers) 
{
	int ret = 0;

	//Open FIFO (server in O_WRONLY; client in O_RDONLY)
	SE_FICHIER fifo;	
	fifo = SE_ouverture("/tmp/fifo-swcr", O_WRONLY);

	//Client had right combination
	if (right_numbers == S.numbers_per_ticket)
	{
		ret = SE_ecritureEntier(fifo, ticket_pid);
		assert(ret != -1);
		ret = SE_ecritureCaractere(fifo, ' ');
		assert(ret != -1);

		ret = SE_ecritureEntier(fifo, 1);
		assert(ret != -1);
		ret = SE_ecritureCaractere(fifo, ' ');
		assert(ret != -1);

		ret = SE_ecritureEntier(fifo, S.earnings[1][S.numbers_per_ticket - right_numbers]);
		assert(ret != -1);
		ret = SE_ecritureCaractere(fifo, ' ');
		assert(ret != -1);
	}
	//Client had wrong combination but some numbers were right
	else if (right_numbers > 0 && right_numbers != S.numbers_per_ticket)
	{
		ret = SE_ecritureEntier(fifo, ticket_pid);
		assert(ret != -1);
		ret = SE_ecritureCaractere(fifo, ' ');
		assert(ret != -1);

		ret = SE_ecritureEntier(fifo, 0);
		assert(ret != -1);
		ret = SE_ecritureCaractere(fifo, ' ');
		assert(ret != -1);

		ret = SE_ecritureEntier(fifo, S.earnings[1][S.numbers_per_ticket - right_numbers]);
		assert(ret != -1);
		ret = SE_ecritureCaractere(fifo, ' ');
		assert(ret != -1);
	}
	//Client had all numbers wrong
	else
	{
		ret = SE_ecritureEntier(fifo, ticket_pid);
		assert(ret != -1);
		ret = SE_ecritureCaractere(fifo, ' ');
		assert(ret != -1);

		ret = SE_ecritureEntier(fifo, 0);
		assert(ret != -1);
		ret = SE_ecritureCaractere(fifo, ' ');
		assert(ret != -1);

		ret = SE_ecritureEntier(fifo, 0);
		assert(ret != -1);
		ret = SE_ecritureCaractere(fifo, ' ');
		assert(ret != -1);
	}

	//Close FIFO
	ret = SE_fermeture(fifo);
	assert(ret != -1);

	//Check if winning_combination was found
	if (right_numbers == S.numbers_per_ticket)
		return 1;

	return 0;
}


//Server reads and treats client's TICKET informations from FIFO
// parameter 	S 					SERVER_SETTINGS data structure that will
//									read and treat client's TICKET
void read_server(SERVER_SETTINGS S)
{
	int ret = 0;

	//Initialize variables to receive client's TICKET informations
	int ticket_pid;
	int *ticket_2_read;
	ticket_2_read = malloc(sizeof(int) * S.numbers_per_ticket);
	assert(ticket_2_read != NULL);

	//Create and open FIFO (server in O_RDONLY; client in O_WRONLY)
	mkfifo("/tmp/fifo-srcw", 0666);
	SE_FICHIER fifo;	
	fifo = SE_ouverture("/tmp/fifo-srcw", O_RDONLY);

	sleep(1);
	//Read client's TICKET PID from FIFO
	ret = SE_lectureEntier(fifo, &ticket_pid);
	assert(ret != -1);

	//Read client's TICKET numbers from FIFO
	for (int i = 0; i < S.numbers_per_ticket; ++i) 
	{
		ret = SE_lectureEntier(fifo, &ticket_2_read[i]);
		assert(ret != -1);
	}

	//Processing of client's TICKET
	int right_numbers = 0;
	for (int i = 0; i < S.numbers_per_ticket; ++i)
		if (ticket_2_read[i] == S.winning_ticket[i])
			right_numbers++;

	//Close and suppress FIFO
	ret = SE_fermeture(fifo);
	assert(ret != -1);
	ret = SE_suppression("/tmp/fifo-srcw");
	assert(ret != -1);	

	//Closes server if winning combination was found
	//Start server's write function otherwise
	ret = write_server(S, ticket_pid, right_numbers);
	if (ret == 1)
	{
		printf("\nWinning combination was found.\nServer n°%d closed.\n", S.pid);
		free_server(S);
		exit(1);
	}
	else
		read_server(S);
}


//Initialize server settings
// parameter	file 				Config file to read to 
//									initialize the server
void init_server(char *file) 
{
	//Check if a file was passed in arguments
	if (!file)
	{
		printf("ERROR: no config file\n");
		exit(EXIT_FAILURE);
	}

	int ret = 0;

	//Initialize server
	SERVER_SETTINGS S;
	S.pid = (int)getpid();

	//Open config file
	SE_FICHIER cfg_file;
	cfg_file = SE_ouverture(file, O_RDONLY);

	//Read first line of config file (numbers of digits per ticket)
	ret = SE_lectureEntier(cfg_file, &S.numbers_per_ticket);
	assert(ret != -1);

	//Allocate memory for S.winning_ticket
	S.winning_ticket = malloc(sizeof(int) * S.numbers_per_ticket);
	assert(S.winning_ticket != NULL);

	//Read second line of config file (winning ticket)
	for (int i = 0; i < S.numbers_per_ticket; ++i) 
	{
		ret = SE_lectureEntier(cfg_file, &S.winning_ticket[i]);
		assert(ret != -1);
	}

	//Allocate memory for S.earnings
	S.earnings = malloc(sizeof(int) * 2);
	assert(S.earnings != NULL);
	for (int i = 0; i < 2; ++i) 
	{
		S.earnings[i] = malloc(sizeof(int) * S.numbers_per_ticket);
		assert(S.earnings[i] != NULL);
	}

	//Read remaining lines of config file (possible earnings)
	for (int i = 0; i < S.numbers_per_ticket; ++i) 
	{
		ret = SE_lectureEntier(cfg_file, &S.earnings[0][i]);
		assert(ret != -1);
		ret = SE_lectureEntier(cfg_file, &S.earnings[1][i]);
		assert(ret != -1);
	}

	//Show server settings and start server's read function
	show_SERVER_SETTINGS(S);
	read_server(S);
}


/******************************************************************************/
/***** CLIENT *****************************************************************/
/******************************************************************************/

//Show client's informations
// parameter	C 				TICKET data structure to show
void show_client(TICKET C)
{
	printf("Client n°%d plays: \t\t", C.pid);
	for (int i = 0; i < C.npt; ++i) 
		printf("%d ", C.numbers[i]);

	printf("\n");
}


//Free memory of client's TICKET
// parameter 	C 				TICKET data structure to free
void free_client(TICKET C)
{
	free(C.numbers);
}


//Client reads results from server through FIFO
// parameter 	C 					TICKET data structure that
//									receives server's results
void read_client(TICKET C)
{
	int ret = 0;

	//Create and open FIFO (server in O_WRONLY; client in O_RDONLY)
	mkfifo("/tmp/fifo-swcr", 0666);
	SE_FICHIER fifo;
	fifo = SE_ouverture("/tmp/fifo-swcr", O_RDONLY);

	int c_pid, return_value, earnings;

	sleep(1);
	//Read PID from server from FIFO
	ret = SE_lectureEntier(fifo, &c_pid);
	assert(ret != -1);

	//Check if PID read from FIFO is client's PID
	if (C.pid != c_pid)
	{
		printf("\nERROR: Server sent results to wrong client\n");
		free_client(C);
		exit(-1);
	} 

	//Read return_value from FIFO
	ret = SE_lectureEntier(fifo, &return_value);
	assert(ret != -1);

	//return_value = 1; Client had the right combination
	if (return_value == 1)
	{
		printf("\nClient n°%d had the right combination!\n", C.pid);
		ret = SE_lectureEntier(fifo, &earnings);
		assert(ret != -1);
		printf("You won $%d!\n", earnings);
		free_client(C);
		exit(1);
	}

	//return_value = 0; Client had a wrong combination
	if (return_value == 0)
	{
		printf("\nClient n°%d had a wrong combination.\n", C.pid);
		ret = SE_lectureEntier(fifo, &earnings);
		assert(ret != -1);

		//Check if client had some numbers right and won something
		if (earnings != 0)
		{
			printf("You still won $%d!\n", earnings);
			free_client(C);
			exit(0);
		}
		else
		{
			printf("You didn't win anything...\n");
			free_client(C);
			exit(0);
		}
	}	
	else
	{
		printf("\nERROR: client n°%d did not execute correctly\n", C.pid);
		free_client(C);
		exit(-1);
	}

	//Close and suppress FIFO
	ret = SE_fermeture(fifo);
	assert(ret != -1);
	ret = SE_suppression("/tmp/fifo-swcr");
	assert(ret != -1);
}


//Client writes its TICKET informations to FIFO
// parameter 	C 					TICKET data structure to 
//									send through FIFO
void write_client(TICKET C)
{
	int ret = 0;

	sleep(1);
	//Open FIFO(server in O_RDONLY; client in O_WRONLY)
	SE_FICHIER fifo;	
	fifo = SE_ouverture("/tmp/fifo-srcw", O_WRONLY);

	//Check if FIFO opened correctly
	if (fifo.descripteur == -1)
	{
		printf("\nThere is no ongoing lottery.\n");
		exit(2);
	}

	//Write client's TICKET PID to FIFO
	ret = SE_ecritureEntier(fifo, C.pid);
	assert(ret != -1);
	ret = SE_ecritureCaractere(fifo, ' ');
	assert(ret != -1);

	sleep(1);
	//Write client's TICKET numbers to FIFO
	for (int i = 0; i < C.npt; ++i)
	{
		ret = SE_ecritureEntier(fifo, C.numbers[i]);
		assert(ret != -1);
		ret = SE_ecritureCaractere(fifo, ' ');
		assert(ret != -1);
	}

	//Close FIFO
	ret = SE_fermeture(fifo);
	assert(ret != -1);

	//Start client's read function
	printf("Client n°%d sent its ticket to the server.\n", C.pid);
	read_client(C);
}


//Initialize client
// parameter	argc				Number of arguments from command line
// parameter	argv				Arguments
void init_client(int argc, char **argv) 
{
	//Initialize TICKET data structure
	TICKET C;
	C.pid = (int)getpid();
	C.npt = argc - 2;
	C.numbers = malloc(sizeof(int) * C.npt);
	assert(C.numbers != NULL);

	//Get client's numbers from command line
	for (int i = 0; i < C.npt; ++i) 
		C.numbers[i] = atoi(argv[2 + i]);

	//Show client info and start client's write function
	show_client(C);	
	write_client(C);
}


/******************************************************************************/
/***** MAIN FUNCTION **********************************************************/
/******************************************************************************/

int main(int argc, char **argv) 
{
	char buf[2][BUFFER_SIZE];

	strcpy(buf[0], "server");
	strcpy(buf[1], "client");

	//Initialize server if argument is "server"
	if (!strcmp(argv[1], buf[0]))
	{
		printf("\nInitializing server...\n");
		init_server(argv[2]);
	}

	//Initialize client if argument is "client"
	if (!strcmp(argv[1], buf[1]))
	{
		printf("\nInitializing client...\n");
		init_client(argc, argv);
	}

	return 0;
}
