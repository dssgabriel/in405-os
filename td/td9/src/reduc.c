#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

/// Enum for every possible opcodes.
typedef enum {
    OP_SUM,
    OP_AVG,
    OP_MAX,
    OP_MIN,
    OP_TOT
} opcode_t;

/// Represents the arguments of the program.
typedef struct {
    // Number of threads to use
    unsigned int nb_threads;
    // Size of the array used for the reduction
    unsigned int array_size;
    // Opcode to use
    opcode_t code;
} arg_t;

/// Represents the informations needed by the thread.
typedef struct {
    // Integer array to process
    int *array;
    // Starting index
    unsigned int start;
    // Ending index
    unsigned int end;
    // Local result
    int result;
} message_t;

/// Aliases for function pointers.
typedef void *(*func_ptr) (void*);
typedef int (*reduc_ptr) (message_t*, unsigned int);
typedef int (*check_ptr) (int*, unsigned int, int);

/// Thread function that computes the local sum of an array.
///
/// # Argument:
/// * arg - Message passed by the main thread.
void *array_sum(void *arg) {
    message_t *msg = (message_t*)arg;

    msg->result = 0;

    for (unsigned int i = msg->start; i < msg->end; i++) {
        msg->result += msg->array[i];
    }

    return NULL;
}

/// Returns the global sum of an array (reduction).
///
/// # Arguments:
/// * msg - Messages from the threads execution containing local results.
/// * nb_threads	- Number of threads (i.e. number of messages).
int reduc_sum(message_t *msg, unsigned int nb_threads) {
    int res = 0;

    for (unsigned int i = 0; i < nb_threads; i++) {
        res += msg[i].result;
    }

    return res;
}

/// Thread function that computes the local average of an array.
///
/// # Argument:
/// * arg - Message passed by the main thread.
void *array_avg(void *arg) {
    message_t *msg = (message_t*)arg;

    msg->result = 0;

    for (unsigned int i = msg->start; i < msg->end; i++) {
        msg->result += msg->array[i];
    }

    return NULL;
}

/// Returns the global average of an array (reduction).
///
/// # Arguments:
/// * msg - Messages from the threads execution containing local results.
/// * nb_threads	- Number of threads (i.e. number of messages).
int reduc_avg(message_t *msg, unsigned int nb_threads) {
    unsigned int array_size = 0;
    int res = 0;

    for (unsigned int i = 0; i < nb_threads; i++) {
        res += msg[i].result;
        array_size += msg[i].end - msg[i].start;
    }

    return (array_size) ? res / array_size : res;
}

/// Thread function that computes the local maximum of an array.
///
/// # Argument:
/// * arg - Message passed by the main thread.
void *array_max(void *arg) {
    message_t *msg = (message_t*)arg;

    msg->result = 0;

    for (unsigned int i = msg->start; i < msg->end; i++) {
        msg->result = (msg->result < msg->array[i] ?
            msg->array[i] : msg->result
        );
    }

    return NULL;
}

/// Returns the global maximum of an array.
///
/// # Arguments:
/// * msg - Messages from the threads execution containing local results.
/// * nb_threads	- Number of threads (i.e. number of messages).
int reduc_max(message_t *msg, unsigned int nb_threads) {
    int res = 0;

    for (unsigned int i = 0; i < nb_threads; i++) {
        res = (res < msg[i].result ?
            msg[i].result : res
        );
    }

    return res;
}

/// Thread function that computes the local minimum of an array.
///
/// # Argument:
/// * arg - Message passed by the main thread.
void *array_min(void *arg) {
    message_t *msg = (message_t*)arg;

    msg->result = 101;

    for (unsigned int i = msg->start; i < msg->end; i++) {
        msg->result = (msg->result > msg->array[i] ?
            msg->array[i] : msg->result
        );
    }

    return NULL;
}

/// Returns the global maximum of an array.
///
/// # Arguments:
/// * msg - Messages from the threads execution containing local results.
/// * nb_threads	- Number of threads (i.e. number of messages).
int reduc_min(message_t *msg, unsigned int nb_threads) {
    int res = 101;

    for (unsigned int i = 0; i < nb_threads; i++) {
        res = (res > msg[i].result ?
            msg[i].result : res
        );
    }

    return res;
}

/// Verification function that checks that the sum of an array is correct.
///
/// # Arguments:
/// * array - Array of integers.
/// * array_size - Size of the array.
/// * result - Reduction result.
int check_sum(int *array, unsigned int array_size, int result) {
    int tmp = 0;

    for (unsigned int i = 0; i < array_size; i++) {
        tmp += array[i];
    }

    return result == tmp;
}

/// Verification function that checks that the average of an array is correct.
///
/// # Arguments:
/// * array - Array of integers.
/// * array_size - Size of the array.
/// * result - Reduction result.
int check_avg(int *array, unsigned int array_size, int result) {
    int tmp = 0;

    for (unsigned int i = 0; i < array_size; i++) {
        tmp += array[i];
    }

    return (array_size) ? result == (tmp / array_size) : tmp;
}

/// Verification function that checks that the maximum of an array is correct.
///
/// # Arguments:
/// * array - Array of integers.
/// * array_size - Size of the array.
/// * result - Reduction result.
int check_max(int *array, unsigned int array_size, int result) {
    int tmp = 0;

    for (unsigned int i = 0; i < array_size; i++) {
        tmp = (tmp < array[i] ? array[i] : tmp);
    }

    return result == tmp;
}

/// Verification function that checks that the minimum of an array is correct.
///
/// # Arguments:
/// * array - Array of integers.
/// * array_size - Size of the array.
/// * result - Reduction result.
int check_min(int *array, unsigned int array_size, int result) {
    int tmp = 101;

    for (unsigned int i = 0; i < array_size; i++) {
        tmp = (tmp > array[i] ? array[i] : tmp);
    }

    return result == tmp;
}

/// Analyzes the arguments passed on program execution.
/// Exits if there is not 4 arguments.
/// Returns a structure arg_t with the specified arguments.
///
/// # Arguments:
/// * argc - Number of arguments
/// * argv - Array of strings holding the arguments.
arg_t get_args(int argc, char ** argv) {
    if (argc != 4) {
        printf("Usage: %s m n [+ | / | M | m]\n", argv[0]);
        exit(-1);
    }

    arg_t a;
    a.nb_threads = atoi(argv[1]);
    a.array_size = atoi(argv[2]);

    switch (argv[3][0]) {
        case '+': a.code = OP_SUM; break;
        case '/': a.code = OP_AVG; break;
        case 'M': a.code = OP_MAX; break;
        case 'm': a.code = OP_MIN; break;
        default: a.code = OP_TOT;
    }

    return a;
}

/// Gets the thread function depending on the specified opcode.
///
/// # Arguments:
/// * opcode - Operation code to apply on the array through a thread function.
func_ptr decode_opcode(const opcode_t o) {
    switch (o) {
        case OP_SUM: return array_sum;
        case OP_AVG: return array_avg;
        case OP_MAX: return array_max;
        case OP_MIN: return array_min;
        default:
            printf("Error: The opcode is not known\n");
            exit(-1);
    }
}

/// Gets the reduction function depending on the specified opcode.
///
/// # Arguments:
/// * opcode - Operation code to apply on the array through a reduction function.
reduc_ptr decode_opcode_reduc(const opcode_t o) {
    switch (o) {
        case OP_SUM: return reduc_sum;
        case OP_AVG: return reduc_avg;
        case OP_MAX: return reduc_max;
        case OP_MIN: return reduc_min;
        default:
            printf("Error: the opcode is not known\n");
            exit(-1);
    }
}

/// Gets the verification function depending on the specified opcode.
///
/// # Arguments:
/// * opcode - Operation code to apply on the array through a verification function.
check_ptr decode_opcode_check(const opcode_t o) {
    switch (o) {
        case OP_SUM: return check_sum;
        case OP_AVG: return check_avg;
        case OP_MAX: return check_max;
        case OP_MIN: return check_min;
        default:
            printf("Error: the opcode is not known\n");
            exit(-1);
    }
}

/// Initialization of the random integer array.
///
/// # Arguments:
/// * array_size - Size of the array.
int *init_array(unsigned int array_size) {
    int *arr = malloc(array_size * sizeof(int));

    if (arr) {
        for (unsigned int i = 0; i < array_size; i++) {
            arr[i] = 1 + rand () % 100;
        }

        return arr;
    } else {
        return printf("Error: cannot allocate memory for the array\n"), NULL;
    }
}

/// Performs the multi-threaded reduction.
/// Initializes the different variables used by the threads and creates them.
/// Once the threads are done executing, calls the check function.
///
/// # Arguments:
/// * arg - Arguments passed to the program.
void main_prog(const arg_t arg) {
    // Some variable declarations
    pthread_t *tid;
    message_t *msg;
    func_ptr thread_func = decode_opcode(arg.code);
    reduc_ptr reduc_func = decode_opcode_reduc(arg.code);

    // Memory allocations
    tid = malloc(arg.nb_threads * sizeof(pthread_t));
    msg = malloc(arg.nb_threads * sizeof(message_t));

    if (tid && msg) {
        // Array initialization
        int *arr = init_array(arg.array_size);
        // Thread creation
        for (unsigned int i = 0; i < arg.nb_threads; i++) {
            msg[i].array = arr;
            msg[i].start = i * arg.array_size / arg.nb_threads;
            msg[i].end = (i + 1) * arg.array_size / arg.nb_threads;
            pthread_create(tid + i, NULL, thread_func, msg + i);
        }

        // Join once the threads are done executing
        for (unsigned int i = 0; i < arg.nb_threads; i++) {
            pthread_join(tid[i], NULL);
        }

        // Reduction and print of the result
        int res = (*reduc_func) (msg, arg.nb_threads);
        printf("The result is : %d\n", res);

        // Verification of the result
        if ((*(decode_opcode_check(arg.code))) (arr, arg.array_size, res)) {
            printf ("It is correct\n");
        } else {
            printf ("It is false\n");
        }

        // Memory deallocations
        free(tid);
        free(msg);
        free(arr);
    } else {
        printf("Error: cannot allocate memory for tid and msg\n");
        exit(-1);
    }
}

/// Main function
/// Analyzes the arguments passed at runtime.
/// Executes the main program function using multi-threaded reduction.
///
/// # Arguments:
/// * argc - Number of arguments
/// * argv - Array of string holding the arguments.
int main (int argc, char ** argv) {
    srand(time(NULL));
    arg_t a;

    a = get_args(argc, argv);
    main_prog(a);

    return 0;
}
