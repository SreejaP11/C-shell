#ifndef __SEEK_H
#define __SEEK_H

int seek_directory(const char* search_target, char* target_directory, int find_files, int find_dirs, int execute_flag, char* main_target_dir, const char* home_dir, const char* prev_dir, int* match_count, char* matched_path);
void execute(char* matched_path, const char* main_target_dir, int find_dirs, int find_files);

#endif