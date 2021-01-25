#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>


void mode_to_letters(int mode, char str[]) {
    strcpy(str, "----------");          /* default = no perms   */

    if(S_ISDIR(mode))   str[0] = 'd';   /* directory    */
    if(S_ISCHR(mode))   str[0] = 'c';   /* char devices */
    if(S_ISBLK(mode))   str[0] = 'b';   /* block devices*/
    if(S_ISREG(mode))   str[0] = '-';   /* regular file */         
    if(S_ISFIFO(mode))  str[0] = 'f';   /* fifo         */         
    if(S_ISLNK(mode))   str[0] = 'l';   /* symlink      */         
    if(S_ISSOCK(mode))  str[0] = 's';   /* socket       */         

    if(mode & S_IRUSR)  str[1] = 'r';   /* 3 bits for user  */
    if(mode & S_IWUSR)  str[2] = 'w';
    if(mode & S_IXUSR)  str[3] = 'x';

    if(mode & S_IRGRP)  str[4] = 'r';   /* 3 bits for group */
    if(mode & S_IWGRP)  str[5] = 'w';
    if(mode & S_IXGRP)  str[6] = 'x';

    if(mode & S_IROTH)  str[7] = 'r';   /* 3 bits for other */
    if(mode & S_IWOTH)  str[8] = 'w';
    if(mode & S_IXOTH)  str[9] = 'x';
}

char *uid_to_name(uid_t uid) {
    struct passwd *getpwuid(), *pw_ptr;
    static char numstr[10];

    if(!(pw_ptr = getpwuid(uid))){
        sprintf(numstr, "%d", uid);
        return numstr;
    }
    else
        return pw_ptr->pw_name;
}

char *gid_to_name(gid_t gid) {
    struct group *getgrgid(), *grp_ptr;
    static  char numstr[10];

    if(!(grp_ptr = getgrgid(gid))) {
        sprintf(numstr,"%d", gid);
        return numstr;
    }
    else
        return grp_ptr->gr_name;
}


int main(int argc, char* argv[]) {
    DIR *mydir;
    struct dirent *myfile;
    struct stat mystat;

    char buffer[512];
    char modestr[11];
    mydir = opendir(argv[1]);
    while((myfile = readdir(mydir))) {
        sprintf(buffer, "%s/%s", argv[1], myfile->d_name);
        stat(buffer, &mystat);
        mode_to_letters(mystat.st_mode, modestr);
        printf("%s\t", modestr);
        printf("%s\t", uid_to_name(mystat.st_uid));
        printf("%s\t", gid_to_name(mystat.st_gid));
        printf("%ld\t", mystat.st_size);
        printf(" %s\n", myfile->d_name);
    }
    while((myfile = readdir(mydir))) {
        system("ls -l");
    }
    closedir(mydir);
}
