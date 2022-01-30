#define _GNU_SOURCE
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include "string.h"
#include <dirent.h>
#include <sys/stat.h>
#include <ncurses.h>

struct todo_item {
    char *msg;
    int priority;
    char file[256];
    int line;
};

struct todo_node {
    struct todo_item item;
    struct todo_node *next;
};

struct todo_node *head = NULL;

const char *bad_dirs[] = {".", "..", ".git", "objs"};

bool should_read_dir(char *path) {
    int num_of_bad_dirs = sizeof(bad_dirs) / sizeof(const char *);
    for(int i = 0; i < num_of_bad_dirs; i++) {
        if(strcmp(path, bad_dirs[i]) == 0) return false;
    }
    return true;
}

void insert_node(struct todo_node *node) {
    if(!head) 
        head = node;
    else {
        if(node->item.priority > head->item.priority) {
            node->next = head;
            head = node;
        }
        else {
            struct todo_node *itr = head;
            do {
                if(itr->next == NULL) {
                    itr->next = node;
                    break;
                }
                else if(node->item.priority > itr->next->item.priority) {
                    node->next = itr->next;
                    itr->next = node;
                    break;
                }
                itr = itr->next;
            } while(itr);
        }
    }
}

void search_file(char *path) {
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
            struct todo_node *node = malloc(sizeof(struct todo_node));
            node->item.msg = malloc(sizeof(char) * len);
            strncpy(node->item.msg, line, len);
            char *split = strtok(line, " ");
            while(split) {
                char *ptr = strstr(split, "TODO");
                if(ptr) {
                    int prio = 0;
                    ptr += 3;
                    while(*ptr == 'O') {
                        prio++;
                        ptr++;
                    }
                    node->item.priority = prio;
                }
                split = strtok(NULL, " ");
            }
            strcpy(node->item.file, path);
            node->item.line = line_count;
            node->next = NULL;
            insert_node(node); 
        }
    }
    fclose(fp);
    free(line);
}

int read_dir(char *path) {
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

void print_list() {
    struct todo_node *itr = head;
    while(itr) {
        printw("TODO %d at %s:%d\n%s",itr->item.priority, itr->item.file, itr->item.line, itr->item.msg);
        itr = itr->next;
    }
    refresh();
}

int main(void) {
    initscr();
    addstr("DOTO\n");
    refresh();
    read_dir(".");
    print_list();
    getch();
    endwin();
    return EXIT_SUCCESS;
}
