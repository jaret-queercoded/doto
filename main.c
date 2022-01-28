#define _GNU_SOURCE
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include "string.h"
#include <dirent.h>
#include <sys/stat.h>

const char *bad_dirs[] = {".", "..", ".git", "objs"};

bool should_read_dir(char *path) {
    int num_of_bad_dirs = sizeof(bad_dirs) / sizeof(const char *);
    for(int i = 0; i < num_of_bad_dirs; i++) {
        if(strcmp(path, bad_dirs[i]) == 0) return false;
    }
    return true;
}

void search_file(char *path) {
    printf("Reading: %s\n", path);
    FILE *fp;
    char *line = NULL;
    size_t len = 0;
    int read = 0;
    unsigned int line_count = 0;

    fp = fopen(path, "r");
    if(fp == NULL)
        return;
    while((read = getline(&line, &len, fp)) && read >= 0) {
        line_count++;
        if(strstr(line, "TODO")) {
            printf("Line contains todo: %s", line);
        }
    }
    fclose(fp);
    free(line);
}

int read_dir(char *path) {
    printf("Reading dir: %s\n", path);
    DIR *dir;
    struct dirent *entry;
    struct stat file_stat;
    dir = opendir(path);
    if(dir == NULL) {
        puts("Unable to read dir");
        return 1;
    }
    while((entry = readdir(dir))) {
        char full_path[256] = "";
        strcat(full_path, path);
        strcat(full_path, "/");
        strcat(full_path, entry->d_name);
        stat(full_path, &file_stat);
        if(S_ISDIR(file_stat.st_mode)) {
            if(should_read_dir(entry->d_name))
                read_dir(full_path);
        }
        else {
            search_file(full_path);
        }
    }
    closedir(dir);
    return 0;
}

int main(void) {
    read_dir(".");
    return 0;
}
