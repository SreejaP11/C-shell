#include "headers.h"

int seek_directory(const char* search_target, char* target_directory, int find_files, int find_dirs, int execute_flag, char* main_target_dir, const char* home_dir, const char* prev_dir, int* match_count, char* matched_path) 
{
    if(target_directory[0] == '~' && target_directory[1] == '/'){
        char next_path[2048];
        strcpy(next_path, target_directory + 2);
        char respective_path[MAX_LENGTH];
        snprintf(respective_path, sizeof(respective_path), "%s/%s", home_dir, next_path);
        strcpy(target_directory, respective_path);
        strcpy(main_target_dir, respective_path);
    }
    else if(target_directory[0] == '-' && target_directory[1] == '/'){
        char next_path[2048];
        strcpy(next_path, target_directory + 2);
        char respective_path[MAX_LENGTH];
        snprintf(respective_path, sizeof(respective_path), "%s/%s", prev_dir, next_path);
        strcpy(target_directory, respective_path);
        strcpy(main_target_dir, respective_path);
    }
    int error = 0;
    DIR *dir = opendir(target_directory);
    if(!dir){
        error = 1;
        perror("\033[0;31mopendir\033[0m");
        return error;
    }
    struct dirent *entry;
    struct stat fileStat;
    char full_path[MAX_LENGTH];
    char relative_path[MAX_LENGTH];
    char cwd[MAX_LENGTH];
    
    // Get the current working directory
    if(getcwd(cwd, sizeof(cwd)) == NULL){
        error = 1;
        perror("\033[0;31mgetcwd\033[0m");
        closedir(dir);
        return error;
    }

    while((entry = readdir(dir)) != NULL){
        // Skip "." and ".." directories
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        snprintf(full_path, sizeof(full_path), "%s/%s", target_directory, entry->d_name);

        // Skip if unable to retrieve stats
        if (stat(full_path, &fileStat) != 0) {
            perror("\033[0;31mstat\033[0m");
            continue;
        }

        // Check if the current entry matches the search search_target
        if (strncmp(entry->d_name, search_target, strlen(search_target)) == 0) {
            if ((S_ISDIR(fileStat.st_mode) && find_dirs) || (S_ISREG(fileStat.st_mode) && find_files)) {
                (*match_count)++;
                // Construct the relative path based on the current working directory
                if(strcmp(main_target_dir, home_dir) == 0){
                    snprintf(relative_path, sizeof(relative_path), ".%s/%s", target_directory + strlen(home_dir), entry->d_name);
                }
                else if(strcmp(main_target_dir, home_dir) == 0){
                    snprintf(relative_path, sizeof(relative_path), ".%s/%s", target_directory + strlen(prev_dir), entry->d_name);
                }
                else if (strcmp(cwd, target_directory) == 0) {
                    snprintf(relative_path, sizeof(relative_path), ".%s/%s", target_directory + strlen(main_target_dir), entry->d_name);
                }
                else {
                    snprintf(relative_path, sizeof(relative_path), ".%s/%s", target_directory + strlen(main_target_dir), entry->d_name);
                }

                if (S_ISDIR(fileStat.st_mode)) {
                    printf("\033[1;34m%s\033[0m\n", relative_path);
                } else if (access(full_path, X_OK) == 0) {
                    printf("\033[1;32m%s\033[0m\n", relative_path);
                } else {
                    printf("\033[1;32m%s\033[0m\n", relative_path);
                }
                strcpy(matched_path, relative_path);
            }
        }
        // If it's a directory, recurse into it
        if (S_ISDIR(fileStat.st_mode)) {
            seek_directory(search_target, full_path, find_files, find_dirs, execute_flag, main_target_dir, home_dir, prev_dir, match_count, matched_path);
        }
    }
    closedir(dir);
    return error;
}

void execute(char* matched_path, const char* main_target_dir, int find_dirs, int find_files)
{
    struct stat fileStat;
    char absolute_path[MAX_LENGTH];

    // Construct the absolute path relative to the main target directory
    snprintf(absolute_path, sizeof(absolute_path), "%s/%s", main_target_dir, matched_path);

    if(stat(absolute_path, &fileStat) != 0){
        perror("\033[0;31mstat\033[0m");
        return;  
    }

    if(S_ISDIR(fileStat.st_mode)){
        // Change directory relative to the main target directory
        if(chdir(absolute_path) != 0){
            if(errno == EACCES){
                printf("\033[0;31mMissing permissions for task!\033[0m\n");
            } 
            else{
                perror("\033[0;31mchdir\033[0m");
            }
            return;
        }
    }
    else if(S_ISREG(fileStat.st_mode)){
        // Open file relative to the main target directory
        FILE *file = fopen(absolute_path, "r");
        if(!file){
            if(errno == EACCES){
                printf("\033[0;31mMissing permissions for task!\033[0m\n");
            } 
            else {
                perror("\033[0;31mfopen\033[0m");
            }
            return;
        }
        char line[MAX_LENGTH];
        while(fgets(line, sizeof(line), file) != NULL){
            printf("%s", line);
        }
        fclose(file);
    }
}