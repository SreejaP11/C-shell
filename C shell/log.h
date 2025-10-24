#ifndef __LOG_H
#define __LOG_H

#define MAX_NO_COMMANDS 15
#define FILENAME "commands.txt"

extern char commands_file[4200];
void add_to_file(char* command, char* commands[], int* count);
void clear_file(char* commands[], int* count);
void print_file(char* commands[], int count);
void load_file(char* commands[], int* count);

#endif