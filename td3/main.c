#include "io_file.h"



// Show file content
// @param 	PATH
// @return	-1 if FAILURE, 0 else
int print(const char *path) {
	IO_FILE file = IO_open(path, O_RDONLY);
	if((file.path == NULL) || (file.desc == -1)) 
		return -1;

	char print_char;
	int success = 1;
	do {
		success = IO_char_read(file, &print_char);
		if(success) printf("%c", print_char);
	} while(success > 0);

	IO_close(file);
	return 0;
}


// Copy file
// @param		src PATH
// @param		destination PATH
// @return		-1 if FAILURE, 0 else
int copy(const char *path1, const char *path2) {
	IO_FILE file1 = IO_open(path1, O_RDONLY);
	if((file1.path == NULL) || (file1.desc == -1)) 
		return -1;

	char *buffer = malloc(SIZE_BUF*sizeof(char));
	if(!buffer) 
		exit(FAILED_PTR_NULL);
	int success, pos_read;
	char char_read;
	success = 1; 
	pos_read = 0;

	do {
		if(pos_read == SIZE_BUF-1) buffer = (char *) 
			realloc(buffer, 1.5*SIZE_BUF*sizeof(char));
		success = IO_char_read(file1, &char_read);
		if(success) {
			buffer[pos_read] = char_read;
			pos_read++;
		}
	} while(success > 0);

	IO_close(file1);

	IO_FILE file2 = IO_open(path2, O_WRONLY);
	if((file2.path == NULL) || (file2.desc == -1)) 
		return -1;
	int pos_write = 0;
	success = 1;

	do {
		success = IO_char_write(file2, buffer[pos_write]);
		pos_write++;
	} while((pos_write < pos_read) && (success > 0));

	IO_close(file2);
	free(buffer);
	return 0;
}


// Move file
// @param		src PATH
// @param		destination PATH
// @return		-1 if FAILURE, 0 else
int move(const char *path1, const char *path2){
	int success_open = copy(path1, path2);
	IO_remove(path1);
	return success_open;
}


// Comparison between two files
// @param		src PATH
// @param		destination PATH
// @return		-1 if FAILURE, 1 if files are the same, 0 else
int are_the_same(const char *path1, const char *path2) {
	IO_FILE file1 = IO_open(path1, O_RDONLY);
	IO_FILE file2 = IO_open(path2, O_RDONLY);

	char *buffer = malloc(SIZE_BUF*sizeof(char));
	if(!buffer) 
		exit(FAILED_PTR_NULL);
	int success, pos_read;
	char char_read;
	success = 1;
	pos_read = 0;

	do {
		if(pos_read == SIZE_BUF-1) buffer = (char *) 
			realloc(buffer, 1.5*SIZE_BUF*sizeof(char));
		success = IO_char_read(file1, &char_read);
		if(success) {
			buffer[pos_read] = char_read;
			pos_read++;
		}
	} while(success > 0);

	char *buffer2 = malloc(SIZE_BUF*sizeof(char));
	if(!buffer2) 
		exit(FAILED_PTR_NULL);
	int success2, pos_read2;
	char char_read2;
	success2 = 1; 
	pos_read2 = 0;

	do {
		if(pos_read2 == SIZE_BUF-1) buffer2 = (char *) 
			realloc(buffer2, 1.5*SIZE_BUF*sizeof(char));

		success2 = IO_char_read(file2, &char_read2);
		if(success2) {
			buffer2[pos_read2] = char_read2;
			pos_read2++;
		}
	} while(success2 > 0);

	if(!strcmp(buffer, buffer2)) {
		free(buffer); 
		free(buffer2);
		return 1;
	}

	return 0;

}


void check_print() {
	int nstdout = creat("test2.txt", 0644);
	int tmp;

	system("echo 'This file\nis\n\nGREAT!\n' >test1.txt");

	// dup: duplicate the file descriptor
	// fileno: return file descripterr of his parameter
	tmp = dup(fileno(stdout));

	// empty stdout buffer
	fflush(NULL);
	dup2(nstdout, fileno(stdout));

	print("test1.txt");

	// empty stdout buffer
	fflush(NULL);
	dup2(tmp, fileno(stdout));

	close(nstdout); 
	close(tmp);
	int val_diff = system("diff test1.txt test2.txt >diff.log");

	if(!val_diff)
		printf("'print()' test has succeeded.\n");
	else
		printf("'print()' test has failed.\n");

	// system("rm -f diff.log *.txt");
}


void check_copy() {
	system("echo 'This file\nis\n\nGREAT!\n' >test1.txt");

	copy("test1.txt", "test2.txt");

	if (!system("diff test1.txt test2.txt >diff.log"))
		printf("'copy()' test has succeeded.\n");
	else
		printf("'copy()' test has failed.\n");

	// system("rm -f diff.log *.txt");
}


void check_move() {
	system("echo 'This file\nis\n\nGREAT!\n' >test1.txt");
	system("cat test1.txt >test2.txt");

	move("test2.txt", "test3.txt");

	// if(system("[ -e test2.txt ]") != 0)
	int file2_exist = system("test -e test2.txt");
	if(!file2_exist)
		printf("'move()' test has failed.\n");
	else if (!system("diff test1.txt test3.txt >diff.log"))
		printf("'move()' test has succeeded.\n");
	else
		printf("'move()' test has failed.\n");

	// system("rm -f diff.log *.txt");
}


void check_are_the_same () {
	system("echo 'This file\nis\n\nGREAT!\n' >test1.txt");
	system("echo 'This file\nis\n\nGREAT!\n' >test2.txt");
	system("echo 'This file\nis\n\nGREAT!\n\n' >test3.txt");
	system("echo 'This fil\nis\n\nGREAT!\n' >test4.txt");
	system("echo 'This file\nis\nGREAT!\n' >test5.txt");

	if(!are_the_same("test1.txt", "test2.txt"))
		printf("'are_the_same()' test#1 has failed.\n");
	else if(are_the_same("test1.txt", "test3.txt"))
		printf("'are_the_same()' test#2 has failed.\n");
	else if(are_the_same("test2.txt", "test4.txt"))
		printf("'are_the_same()' test#3 has failed.\n");
	else if(are_the_same("test3.txt", "test5.txt"))
		printf("'are_the_same()' test#4 has failed.\n");
	else
		printf("'are_the_same()' test has succeeded.\n");

	// system("rm -f *.txt");
}


int main(int argc, char **argv) {
	puts("\n");
	// check_print();
	// check_copy();
	// check_move();
	check_are_the_same();

	return 0;
}
