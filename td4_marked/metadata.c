#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pwd.h>
#include <dirent.h>

void print_filetype(const char * path) {
	struct stat buf;
	lstat(path, &buf);

	if (S_ISSOCK(buf.st_mode) )
		printf("sock");
	else if (S_ISLNK(buf.st_mode) )
		printf("link");
	else if (S_ISREG(buf.st_mode) )
		printf("file");
	else if (S_ISBLK(buf.st_mode) || S_ISCHR(buf.st_mode) )
		printf("devc");
	else if (S_ISDIR(buf.st_mode) )
		printf("repy");
	else if (S_ISFIFO(buf.st_mode) )
		printf("fifo");
	else
		printf("und.");
}

void print_mode(const char * path) {
	struct stat buf;
	lstat(path, &buf);

	printf("%c%c%c%c%c%c%c%c%c",
		(S_IRUSR & buf.st_mode) ? 'r' : '-',
		(S_IWUSR & buf.st_mode) ? 'w' : '-',
		(S_IXUSR & buf.st_mode) ? 'x' : '-',
		(S_IRGRP & buf.st_mode) ? 'r' : '-',
		(S_IWGRP & buf.st_mode) ? 'w' : '-',
		(S_IXGRP & buf.st_mode) ? 'x' : '-',
		(S_IROTH & buf.st_mode) ? 'r' : '-',
		(S_IWOTH & buf.st_mode) ? 'w' : '-',
		(S_IXOTH & buf.st_mode) ? 'x' : '-');
}

void print_owner(const char * path) {
	struct stat buf;
	struct passwd * pwd;
	lstat(path, &buf);
	pwd = getpwuid(buf.st_uid);

	printf("%s", pwd->pw_name);
}

void print_filesize(const char * path) {
	struct stat buf;
	lstat(path, &buf);

	printf("%ld", buf.st_size);
}

int is_directory(const char * path) {
	struct stat buf;
	lstat(path, &buf);
	return S_ISDIR (buf.st_mode);
}

void ls(const char * path) {
	DIR * rep;
	struct dirent * entry;

	if (! is_directory(path) ) {
		printf("'%s' is not a directory\n", path);
		return;
	}

	rep = opendir(path);

	while (entry = readdir(rep)) {
		if (entry->d_name [0] != '.')
			printf("%s ", entry->d_name);
	}

	printf("\n");

	closedir(rep);
}

void ls_detailed(const char *path) {
	DIR * rep;
	struct dirent * entry;

	if (! is_directory(path) ) {
		printf("'%s' is not a directory\n", path);
		return;
	}

	rep = opendir(path);

	while (entry = readdir(rep))
		if (entry->d_name [0] != '.') {
			// "directory" + "/" + "file"
			char *ent_name = calloc(strlen(entry->d_name)
									+ strlen(path) + 1, 1);
			if (!ent_name) {
				printf("path allocation has failed\n");
				closedir(rep);
				return;
			}

			strcat(ent_name, path);
			strcat(ent_name, "/");
			strcat(ent_name, entry->d_name);

			print_filetype(ent_name); printf(" ");
			print_mode(ent_name); printf(" ");
			print_owner(ent_name); printf(" ");
			print_filesize(ent_name); printf(" ");
			printf("%s\n", ent_name);

			free(ent_name);
		}

	closedir(rep);
}

int main (int argc, char **argv) {
	if (argc != 2) {
		printf("Usage:~: ./ls dir_name\n");
		return 1;
	}

	ls_detailed(argv[1]);

	return 0;
}