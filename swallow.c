#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>


int main(int argc, char** argv){
	if (argc < 2){
		fprintf(stderr,"%s: missing operand\n",argv[0]);
		return 1;
	}
	if (mkdir(argv[1],0775) != 0){
		perror("failed to create directory");
		return 1;
	}
	char* current_dir = getcwd(NULL,0);
	if (current_dir == NULL){
		perror("failed to get current directory");
		return 1;
	}
	DIR* current_dir_handle = opendir(current_dir);
	if (current_dir == NULL){
		perror("failed to get current directory handle");
		return 1;
	}
	struct dirent* directory_entry;
	while( (directory_entry = readdir(current_dir_handle)) != NULL) {
		if ( strcmp("..",directory_entry->d_name) == 0  || strcmp(".",directory_entry->d_name) == 0 || strcmp(argv[1],directory_entry->d_name) == 0  ) 
			continue;
		size_t buffer_size = strlen(current_dir) + strlen(directory_entry->d_name) + 1;
		char* buffer = malloc(sizeof(char) * buffer_size);
		if (sprintf(buffer,"%s/%s",current_dir,directory_entry->d_name) < 1){
			perror("failed at alloc buffer for new path");
			return 1;
		}
		struct stat info;
		if (lstat(buffer,&info) != 0){
			perror("failed to get file information");
			return 1;
		}
		if ((info.st_mode & S_IFMT) != S_IFDIR){
			free(buffer);
			continue;
		}
		printf("moving the dir %s into target new dir\n",directory_entry->d_name);
		size_t new_dir_size = strlen(current_dir) + strlen(argv[1]) + strlen(directory_entry->d_name) + 2;
		char* new_dir = malloc(sizeof(char) * new_dir_size);
		if (sprintf(new_dir,"%s/%s/%s",current_dir,argv[1],directory_entry->d_name) < 1){
			perror("failed at alloc buffer for new path");
			return 1;
		}
		rename(buffer,new_dir);
		free(buffer);
		free(new_dir);
	}
	return 0;
}
