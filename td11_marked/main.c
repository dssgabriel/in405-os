#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mem.h"
#include "types.h"

//
int main(int argc, char **argv)
{
	if (argc < 2)
	{
		printf("Error: too few arguments to run %s\n", argv[0]);
		exit(1);
	}

	MEM_CFG cfg = load_cfg(argv[1]);
	print_cfg(cfg);

	MEMORY mem = init_mem(cfg);

	init_thread(cfg, mem);

	return 0;
}
