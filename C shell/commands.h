#ifndef __COMMANDS_H
#define __COMMANDS_H

void execute_custom_commands(char* command_fgbg, char* home_directory, char* cwd, char* prev_directory, char* input, char** commands, int* command_count, char** commands_fgbg, int* command_fgbg_count, int* is_background, int j, int len_in, BgProcess bgProcessList[], int* bg_count, int* skip_prompt);
void execute_system_commands(char* command_fgbg, char** bg_commands, int* bg_count, BgProcess bgProcessList[], int background, int elapsed_time, char* ext_prompt, char* individual_comm);
void reset_redirection(int saved_stdout, int saved_stdin);

#endif