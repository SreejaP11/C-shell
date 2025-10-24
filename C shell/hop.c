#include "headers.h"

int is_directory(const char *path)
{
    struct stat path_stat;
    if(stat(path, &path_stat) != 0){
        perror("\033[0;31mstat\033[0m");
        return 0; // Return 0 if stat failed (e.g., path doesn't exist)
    }
    return S_ISDIR(path_stat.st_mode);
}

void hop(char *path,char *cwd,char *prev_directory, char *home_directory) 
{
    if(path == NULL || strlen(path) == 0){
        struct passwd *pw = getpwuid(getuid());
        if (pw) {
            if (chdir(pw->pw_dir) != 0) {
                perror("\033[0;31mchdir\033[0m");
            }
        } else {
            perror("\033[0;31mgetpwuid\033[0m");
        }
    } 
    else{
        if(strcmp(path, "~") == 0){
            if(chdir(home_directory) != 0){
                perror("\033[0;31mchdir\033[0m");
            }
        } 
        else if(strcmp(path, "..") == 0){
            if(chdir("..") != 0){
                perror("\033[0;31mchdir\033[0m");
            }
        } 
        else if(strcmp(path,"-")==0){
            if(strcmp(prev_directory, "") == 0){
                printf("\033[0;31mERROR: Previous directory (OLDPWD) is not set\033[0m\n");
                return;
            }
            if(chdir(prev_directory) != 0){
                perror("\033[0;31mchdir\033[0m");
            } 
        } 
        else if(strcmp(path,".") == 0){
            chdir(cwd);
        }
        else if(path[0] == '/'){
            if(chdir(path) != 0){
                perror("\033[0;31mchdir\033[0m");
            }
        }
        else if(path[0] == '~' && path[1] == '/'){
            char next_path[2048];
            strcpy(next_path, path + 2);
            char respective_path[MAX_LENGTH];
            snprintf(respective_path, sizeof(respective_path), "%s/%s", home_directory, next_path);
            if(chdir(respective_path) != 0){
                perror("\033[0;31mchdir\033[0m");
                return;
            }
        }
        else if(path[0] == '-' && path[1] == '/'){
            printf("enterd\n");
            char next_path[2048];
            strcpy(next_path, path + 2);
            char respective_path[MAX_LENGTH];
            snprintf(respective_path, sizeof(respective_path), "%s/%s", prev_directory, next_path);
            printf("%s\n", respective_path);
            if(chdir(respective_path) != 0){
                perror("\033[0;31mchdir\033[0m");
                return;
            }
        }
        else{
            if(chdir(path) != 0){
                perror("\033[0;31mchdir\033[0m");
            }
        }
    }
    
    char new_cwd[MAX_LENGTH];
    getcwd(new_cwd,sizeof(new_cwd));
    printf("%s\n",new_cwd);
}
