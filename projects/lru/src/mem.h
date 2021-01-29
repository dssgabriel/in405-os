#include <pthread.h>
#include "types.h"
#include "se_fichier.h"


// Memory configuration data structure
typedef struct config {
	// Number of frames (fast memory)
	u32 nb_frames; 

	// Size of a page
	u32 mem_page_size; 

	// Number of pages (slow memory)
	u32 nb_pages; 

	// Number of child threads running
	u32 nb_threads; 

	// Number of accesses asked for by child threads
	u32 nb_access; 
} MEM_CFG;


// Memory data structure
typedef struct memory {
	// Array for fast memory
	i32 *fast_mem; 

	// Array for slow memory
	i32 *slow_mem; 

	// Array for indices
	i32 *indices;
	
	// Fast memory array size
	u32 fast_mem_size; 
	
	// Slow memory array size
	u32 slow_mem_size; 
} MEMORY;


// Thread data structure
typedef struct thread {
	// Thread ID
	pthread_t tid; 

	// Mutex
	pthread_mutex_t *mut; 

	// Condition
	pthread_cond_t *cnd;

	// Memory config for thread usage
	MEM_CFG cfg;

	// Request hit counter
	u32 hit_counter;
} THREAD;


/***** FUNCTIONS FOR PIPE HANDLING *****/

// Create pipes for thread communication
void create_pipes(const u8 *pipe_in, const u8 *pipe_out);

// Write to pipe
void write_pipe(const u8 *pipe, u32 adr);

// Read value from pipe 
i32 read_pipe(const u8 *pipe);

// Destroy pipes when finished 
void destroy_pipes(const u8 *pipe_in, const u8 *pipe_out);


/**** FREE MEMORY *****/

// Free MEMORY data structure
void free_memory(MEMORY mem);


/***** PRINT DATA TO TERMINAL FUNCTIONS *****/

// Print configuration to terminal
void print_cfg(MEM_CFG cfg);

// Print current memory state
void print_mem_state(MEMORY mem);


/***** FUNCTIONS TO INITIALIZE DATA STRUCTURES *****/

// Load and read config file
MEM_CFG load_cfg(u8 *filename);

// Initialize memory
MEMORY init_mem(MEM_CFG cfg);


/***** THREAD RELATED FUNCTIONS *****/

// LRU algorithm
i32 least_recently_used(MEMORY mem, i32 query);

// Process the thread
void *thread_process(void *arg);

// Initialize thread
void init_thread(MEM_CFG cfg, MEMORY mem);
