#include "atlacan.h"
#include <stdio.h>


/*****************************************************************************/
/*** GLOBAL VARIABLES ********************************************************/
/*****************************************************************************/

// never printed, use in gdb
int count_atl_cat = 0;

// global variable to not reset its value 
// with each recursive call of atl_proper_search()
int best_hint = -1;



/*****************************************************************************/
/*** UTILITY FUNCTIONS (used for printf) *************************************/
/*****************************************************************************/

char *atl_get_uid(enum atl_uid uid) {
	switch(uid) {
		case UID_YOU: 		return "you";
		case UID_CLAYTON: 	return "clayton";
		default:			return "other";
	}
}

char *atl_get_gid(enum atl_gid gid) {
	switch(gid) {
		case GID_EXPLORERS:	return "explorers";
		case GID_ABITHAN:	return "abithans";
		case GID_PERTONTAN:	return "pertontans";
		default:			return "others";
	}
}



/*****************************************************************************/
/*** SEARCH ALGORITHMS *******************************************************/
/*****************************************************************************/

void atl_brute_force() {
	char **file_system;
	int hint_data;
	int num_entry;
	struct atl_stat tmp_stat;

	atl_ls(&file_system, &num_entry);

	for(int i = 0; i < num_entry; i++) {
		atl_stat(file_system[i], &tmp_stat);

		if(!atl_is_link(tmp_stat.mode) && atl_is_file(tmp_stat.mode)) {
			atl_cat(file_system[i], &hint_data);
			count_atl_cat++;
			printf("%d \t%s \t%s \t%zu \t%s \t\tDistance to Atlacan: %d\n",
				tmp_stat.mode, atl_get_uid(tmp_stat.uid), 
				atl_get_gid(tmp_stat.gid),
				tmp_stat.size, tmp_stat.name, hint_data);
		}

		if(atl_is_dir(tmp_stat.mode)) {
			atl_cd(file_system[i]);
			atl_brute_force();
			atl_cd("..");
		}
	}
	atl_free_ls(file_system, num_entry);
}


void atl_proper_search() {
	char **file_system;
	int hint_data;
	int num_entry;
	struct atl_stat tmp_stat;

	atl_ls(&file_system, &num_entry);

	for(int i = 0; i < num_entry; i++) {
		atl_stat(file_system[i], &tmp_stat);

		// cat file only if not a sym link 
		// and file is not Clayton's or Pertontan's
		if(!atl_is_link(tmp_stat.mode) && atl_is_file(tmp_stat.mode)
		&& !(tmp_stat.uid == UID_CLAYTON) 
		&& !(tmp_stat.gid == GID_PERTONTAN)) {
			// first time cat
			if(best_hint == -1) {
				atl_cat(file_system[i], &hint_data);
				count_atl_cat++;
				printf("%d \t%s \t%s \t%zu \t%s \t\tDistance to Atlacan: %d\n",
					tmp_stat.mode, atl_get_uid(tmp_stat.uid), 
					atl_get_gid(tmp_stat.gid),
					tmp_stat.size, tmp_stat.name, hint_data);
				best_hint = hint_data;
			}
			// regular cat
			atl_cat(file_system[i], &hint_data);
			count_atl_cat++;
			// keep best hint in memory
			if(hint_data < best_hint)
				best_hint = hint_data;

			printf("%d \t%s \t%s \t%zu \t%s \t\tDistance to Atlacan: %d\n",
				tmp_stat.mode, atl_get_uid(tmp_stat.uid), 
				atl_get_gid(tmp_stat.gid),
				tmp_stat.size, tmp_stat.name, hint_data);
			
			// break if getting away from target file 
			if(best_hint < hint_data) {
				printf("  You are moving away from Atlacan\n");
				break;
			}
		}
		// cd into directory and recursive call only if 
		// executable directory and is not Pertontan's 
		if(atl_is_dir(tmp_stat.mode) && atl_is_uxmode(tmp_stat.mode) 
		&& tmp_stat.gid != GID_PERTONTAN) {

			atl_cd(file_system[i]);
			atl_proper_search();
			atl_cd("..");
		}
	}
	atl_free_ls(file_system, num_entry);
}



/*****************************************************************************/
/*** MAIN FUNCTION ***********************************************************/
/*****************************************************************************/

int main(int argc, char const *argv[]) {
	atl_init();

	printf("\n\n  Display format of file info:\ntype\tuid\tgid\t\tsize\tfilename\t\thint\n\n");
	printf("  You are in: %s\n", atl_getpwd());

	// atl_brute_force();
	atl_proper_search();

	atl_fini();
	return 0;
}
