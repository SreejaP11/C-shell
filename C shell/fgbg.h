#ifndef __FGBG_H
#define __FGBG_H

void execute_command(char **args, int background, int* elapsed_time, char* ext_prompt, char* command, BgProcess bgProcessList[], int* bg_count);
// void check_background_processes(char* bg_commands[], int bg_count, Process processList[], int processCount);
void sigchld_handler(int sig);
void setup_sigchld_handler();

#endif