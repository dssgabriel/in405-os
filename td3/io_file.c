#include "io_file.h"



IO_FILE IO_open(const char *path, int access) {
	IO_FILE file;

	file.path = NULL;
	file.access = -1;
	file.desc = -1;

	file.path = malloc(strlen(path)*sizeof(char));
	if(!file.path) {
		puts("Error: malloc failed");
		return file;
	}

	int fd, fd2;

	struct stat *buffer=malloc(sizeof(struct stat));
	if(stat(path, buffer) == -1) {
		fd2=creat(path, 0644);
		fd=open(path, access);
	} else
		fd=open(path, access);

	if((fd == -1) || (fd2 == -1)) {
		printf("Error: file creation failed\nfd = %d, fd2 = %d\n", fd, fd2);
		return file;
	}
	strcpy(file.path, path);
	file.access=access;
	file.desc=fd;

	return file;
}


int IO_close(IO_FILE file) {
	free(file.path);
	return close(file.desc);
}


int IO_remove(const char *path) {
	return unlink(path);
}


int IO_char_read(IO_FILE file, char *c) {
	if(file.access == O_RDONLY)
		return read(file.desc, c, 1);
	return -1;
}


int IO_char_write(IO_FILE file, const char c) {
	if(file.access == O_WRONLY)
		return write(file.desc, &c, 1);
	return -1;
}
