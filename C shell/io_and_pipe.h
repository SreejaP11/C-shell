#ifndef __IO_AND_PIPE_H
#define __IO_AND_PIPE_H

void handle_redirection(char **args);
void execute_piped_commands(char* command_fgbg, char* home_directory, char* cwd, char* prev_directory, char* input, char** commands, int* command_count, char** commands_fgbg, int* command_fgbg_count, int* is_background, char** bg_commands, int* bg_count, int j, int len_in, BgProcess bgProcessList[], int background, int elapsed_time, char* ext_prompt, char* individual_comm, int custom, int* skip_prompt);

#endif