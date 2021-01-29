#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "mem.h"
#include "se_fichier.h"
#include "types.h"


const u8 *pipe_in = "/tmp/pipe_in";
const u8 *pipe_out = "/tmp/pipe_out";

/***************************************/
/***** FUNCTIONS FOR PIPE HANDLING *****/
/***************************************/

//
void create_pipes(const u8 *pipe_in, const u8 *pipe_out)
{
	// Create pipe_in that will read queries from child threads
    if (mkfifo(pipe_in, 0666) == -1)
    {
        printf("Error: cannot create %s\n", pipe_in);
        exit(1);
    }

    // Create pipe_out that will answer queries from child threads
    if (mkfifo(pipe_out, 0666) == -1) 
    {
        printf("Error: cannot create %s\n", pipe_out);
        exit(1);
    }
}

//
void destroy_pipes(const u8 *pipe_in, const u8 *pipe_out) 
{
	// Unlink /tmp/pipe_in
    if (SE_suppression(pipe_in) == -1)
    {
    	printf("Error: cannot unlink %s\n", pipe_in);
    	exit(1);
    }
    
    // Unlink /tmp/pipe_out
    if (SE_suppression(pipe_out) == -1)
    {
    	printf("Error: cannot unlink %s\n", pipe_out);
    	exit(1);
    }
}

//
void write_pipe(const u8 *pipe, u32 adr)
{
	// Open fifo
	SE_FICHIER fd = SE_ouverture(pipe, O_WRONLY);
    if (fd.descripteur) 
    {
    	// Write adress
    	if (SE_ecritureEntier(fd, adr) == -1)
    	{
    		printf("Error: SE_ecritureEntier() failed\n");
    		destroy_pipes(pipe_in, pipe_out);
    		exit(1);
    	}
    	// Add a space
    	if (SE_ecritureCaractere(fd, ' ') == -1)
    	{
    		printf("Error: SE_ecritureCaractere() failed\n");
    		destroy_pipes(pipe_in, pipe_out);
    		exit(1);
    	}
    	// Close fifo
    	SE_fermeture(fd);
    }
    else
    {
    	printf("Error: cannot open %s\n", pipe_out);
    	destroy_pipes(pipe_in, pipe_out);
    	exit(1);
    }
}

//
i32 read_pipe(const u8 *pipe)
{
	// Open file
	SE_FICHIER fd = SE_ouverture(pipe, O_RDONLY);
	i32 rec_adr;
	if (fd.descripteur)
	{
		// Read adress
		if (SE_lectureEntier(fd, &rec_adr))
		{
			SE_fermeture(fd);
			return rec_adr;
		}
		else
		{
			printf("Error: SE_ecritureEntier failed\n");
			destroy_pipes(pipe_in, pipe_out);
			exit(1);
		}
	}
	else
	{
		printf("Error: cannot open pipe\n");
		destroy_pipes(pipe_in, pipe_out);
		exit(1);
	}
}


/**********************/
/**** FREE MEMORY *****/
/**********************/

void free_memory(MEMORY mem)
{
	free(mem.fast_mem);
	free(mem.slow_mem);
	free(mem.indices);
}


/********************************************/
/***** PRINT DATA TO TERMINAL FUNCTIONS *****/
/********************************************/

//
void print_cfg(MEM_CFG cfg) 
{
	printf("\nCONFIGURATION:\n");
	printf("Number of frames:\t%d\n", cfg.nb_frames);
	printf("Size of memory page:\t%d\n", cfg.mem_page_size);
	printf("Number of pages:\t%d\n", cfg.nb_pages);
	printf("Number of threads:\t%d\n", cfg.nb_threads);
	printf("Number of accesses:\t%d\n\n", cfg.nb_access);
}

void print_mem_state(MEMORY mem)
{
	printf("\nFast memory:\n");
	for (u32 i = 0; i < mem.fast_mem_size; ++i)
		printf("Adress[%d] = %d\n", i, mem.fast_mem[i]);

	printf("\nSlow memory:\n");
	for (u32 i = 0; i < mem.slow_mem_size; ++i)
		printf("Adress[%d] = %d\n", i, mem.slow_mem[i]);

	printf("\nIndices:\n");
	for (u32 i = 0; i < mem.fast_mem_size; ++i)
		printf("Adress[%d] = %d\n", i, mem.indices[i]);
}

/***************************************************/
/***** FUNCTIONS TO INITIALIZE DATA STRUCTURES *****/
/***************************************************/

//
MEM_CFG load_cfg(u8 *filename) 
{
	// If valid pointer
	if (filename)
	{
		SE_FICHIER fp = SE_ouverture(filename, O_RDONLY);
		
		// If file opened successfully
		MEM_CFG cfg;

		// Read number of frames
		if (SE_lectureEntier(fp, &cfg.nb_frames) == -1)
			printf("Error: cannot read number of frames from file\n");

		// Read size of memory page
		if (SE_lectureEntier(fp, &cfg.mem_page_size) == -1)
			printf("Error: cannot read size of memory page from file\n");

		// Read number of pages
		if (SE_lectureEntier(fp, &cfg.nb_pages) == -1)
			printf("Error: cannot read number of pages from file\n");

		// Read number of threads
		if (SE_lectureEntier(fp, &cfg.nb_threads) == -1)
			printf("Error: cannot read number of threads from file\n");

		// Read number of accesses
		if (SE_lectureEntier(fp, &cfg.nb_access) == -1)
			printf("Error: cannot read number of accesses from file\n");

		// Close file and return config 
		SE_fermeture(fp);
		return cfg;
	}
	else 
	{
		printf("Error: file name pointer is NULL\n");
		exit(1);
	}
}

//
MEMORY init_mem(MEM_CFG cfg)
{
	MEMORY mem;

	// Initialize array sizes;
	mem.fast_mem_size = cfg.nb_frames;
	mem.slow_mem_size = cfg.nb_pages;

	// Allocate memory for the fast_mem array
	mem.fast_mem = malloc(sizeof(i32) * mem.fast_mem_size);
	if (!mem.fast_mem)
	{
		printf("Error: cannot allocate memory for fast memory\n");
		exit(1);
	}

	// Allocate memory for the slow_mem array
	mem.slow_mem = malloc(sizeof(i32) * mem.slow_mem_size); 
	if (!mem.slow_mem)
	{
		printf("Error: cannot allocate memory for slow memory\n");
		exit(1);
	}

	// Allocate memory for the array of mem.indices
	mem.indices = malloc(sizeof(i32) * mem.fast_mem_size);
	if (!mem.indices)
	{
		printf("Error: cannot allocate memory for mem.indices\n");
		exit(1);		
	}

	// Initialize fast_mem and mem.indices arrays with -1;
	for (u32 i = 0; i < mem.fast_mem_size; ++i)
	{
		mem.fast_mem[i] = -1;
		mem.indices[i] = -1;
	}

	// Initialize slow_mem array with -1
	for (u32 i = 0; i < mem.slow_mem_size; ++i)
	{
		mem.slow_mem[i] = -1;
	}

	return mem;
}

/************************************/
/***** THREAD RELATED FUNCTIONS *****/
/************************************/

//
i32 least_recently_used(MEMORY mem, i32 request)
{
	// Iterate through the fast_mem array
	for (i32 i = 0; i < mem.fast_mem_size; ++i)
	{
		// If it is unitialized (== -1)
		if (mem.fast_mem[i] == -1)
		{
			// Add the request to the fast_mem and update the indices array
			mem.fast_mem[i] = request;
			for (i32 i = 0; i < mem.fast_mem_size; ++i)
				if (mem.indices[i] != -1)
					mem.indices[i]++;
			mem.indices[i] = 0;
			
			// Return index (where the requested value was found)
			return i;
		}
		// If it is in the fast_meme array
		if (mem.fast_mem[i] == request)
		{
			// Simply update the indices array
			for (i32 i = 0; i < mem.fast_mem_size; ++i)
				if (mem.indices[i] != -1 || mem.indices[i] != 0)
					mem.indices[i]++;
			mem.indices[i] = 0;
			
			// Return index (where the requested value was found)
			return i;
		}
	}
	// If the request was not found in the fast_mem array, iterate through the slow_mem
	for (i32 i = 0; i < mem.slow_mem_size; ++i)
	{
		// If request is in the slow_mem
		if (mem.slow_mem[i] == request)
		{
			// Put its adress back to -1
			mem.slow_mem[i] = -1;

			// Get the oldest page loaded in the fast_mem
			u32 max_index = -1;
			for (i32 i = 0; i < mem.fast_mem_size; ++i)
				if (max_index > mem.indices[i])
					max_index = mem.indices[i];
			
			// Get the oldest request
			i32 oldest_request = mem.fast_mem[max_index];
			// Load the request to the fast_mem
			mem.fast_mem[max_index] = request;
			
			// Load the oldest request back in the slow_mem
			for (i32 i = 0; i < mem.slow_mem_size; ++i)
				if (mem.slow_mem[i] == -1)
					mem.slow_mem[i] = oldest_request;
			// Update the indices array
			for (i32 i = 0; i < mem.fast_mem_size; ++i)
				if (mem.indices[i] != -1)
					mem.indices[i]++;
			mem.indices[max_index] = 0;
			
			// Return -1 (value was not found in the fast_mem, the LRU algotithm had to pull it from slow_mem)
			return -1;
		}
	}
	// If the value was not found in the slow_mem
	// Get the oldest page loaded in the fast_mem
	u32 max_index = -1;
	for (i32 i = 0; i < mem.fast_mem_size; ++i)
		if (max_index > mem.indices[i])
			max_index = mem.indices[i];
		
	// Get the oldest request
	i32 oldest_request = mem.fast_mem[max_index];
	// Load the request to the fast_mem
	mem.fast_mem[max_index] = request;

	// Load the oldest request back in the slow_mem
	for (i32 i = 0; i < mem.fast_mem_size; ++i)
		if (mem.indices[i] != -1)
			mem.indices[i]++;
	// Update the indices array
	for (i32 i = 0; i < mem.slow_mem_size; ++i)
		if (mem.slow_mem[i] == -1)
			mem.slow_mem[i] = oldest_request;
	mem.indices[max_index] = 0;

	// Return -1 (value was not found in the fast_mem)	
	return -1;
}

//
void *thread_process(void *arg) 
{
	srand(getpid());

	THREAD *arg_t = (THREAD *)arg;

	u8 lock;
	for (u32 i = 0; i < arg_t->cfg.nb_access; ++i)
	{
	 	if (!(lock = pthread_mutex_lock(arg_t->mut)))
	 	{
	 		// Generate a random memory adress to request
	 		u32 phy_adr = rand() % arg_t->cfg.nb_pages;
	 		// Write to the pipe
	 		write_pipe(pipe_out, phy_adr);
	 		// Read answer from father thread
	 		u32 lru_ans = read_pipe(pipe_in);
	 		// If answer is not -1, increment the hit_counter
	 		if (lru_ans != -1)
	 			arg_t->hit_counter++;
	 	}
	  	else
	  	{
	    	printf("Error: pthread_mutex_lock failed\n");
	    	exit(1);
	  	}
	  	pthread_mutex_unlock(arg_t->mut);
  	}

	return NULL;
}

//
void init_thread(MEM_CFG cfg, MEMORY mem)
{
	THREAD *thread = malloc(sizeof(THREAD) * cfg.nb_threads);
	pthread_t *tid = malloc(sizeof(pthread_t) * cfg.nb_threads);
	pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
	pthread_cond_t cnd = PTHREAD_COND_INITIALIZER;

	// Create pipes for communication
	create_pipes(pipe_in, pipe_out);

	// Initialize threads and create them
	for (u32 i = 0; i < cfg.nb_threads; ++i) 
	{
		thread[i].mut = &mut;
		thread[i].cnd = &cnd;
		thread[i].cfg = cfg;
		thread[i].hit_counter = 0;
		pthread_create(tid + i, NULL, thread_process, thread + i);
	}

	// Get queries from the threads, run the LRU algorithm and answer back to threads
	for (u32 i = 0; i < cfg.nb_threads * cfg.nb_access; ++i)
	{
		i32 request_t = read_pipe(pipe_out);
		i32 lru_ans = least_recently_used(mem, request_t);
		if (lru_ans != -1)
			lru_ans = lru_ans * cfg.mem_page_size + (request_t % cfg.mem_page_size);
		write_pipe(pipe_in, lru_ans);
	}

	// Join threads
	for (u32 i = 0; i < cfg.nb_threads; ++i)
		pthread_join(tid[i], NULL);

	// Print hit percentage of each thread to terminal
	f32 hit_average = 0;
	for (u32 i = 0; i < cfg.nb_threads; ++i)
	{
		f32 hit_percentage = (f32)thread[i].hit_counter / cfg.nb_access * 100;
		hit_average += hit_percentage;
		printf("Thread n°%ld had a %.2f%s hit ratio\n", tid[i], hit_percentage, "%");
	}
	hit_average = (hit_average) / cfg.nb_threads;
	printf("\nThreads got on average %.2f%s hit ratio\n", hit_average, "%");

	// Free memory
	destroy_pipes(pipe_in, pipe_out);
	
	free_memory(mem);

	free(thread);
	free(tid);
}
 