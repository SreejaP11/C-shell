#include "headers.h"

void printColoredFileType(struct dirent *entry, const char *path) 
{
    char full_path[MAX_LENGTH];
    snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);

    if(entry->d_type == DT_DIR){
        printf("\033[1;34m%s\033[0m\n", entry->d_name); 
    } 
    else if(access(full_path, X_OK) == 0){
        printf("\033[1;32m%s\033[0m\n", entry->d_name); 
    } 
    else{
        printf("%s\n", entry->d_name); 
    }
}

void reveal(bool showAll, bool showDetails, char *path, char* home_directory) 
{
    if(strcmp(path, "..") == 0){
        path = "..";
    }
    else if(strcmp(path, "~") == 0){
        strcpy(path, home_directory);
    }

    DIR *dir = opendir(path);
    if(!dir){
        perror("\033[0;31mopendir\033[0m");
        return;
    }
    struct dirent **entries;
    int numEntries = scandir(path, &entries, NULL, alphasort);
    if(numEntries == -1){
        perror("\033[0;31mscandir\033[0m");
        closedir(dir);
        return;
    }
    long totalBlocks = 0;
    if(showDetails){
        for(int i = 0; i < numEntries; i++) {
            if(!showAll && entries[i]->d_name[0] == '.') {
                continue; // Skip hidden files if `showAll` is false
            }

            char full_path[MAX_LENGTH];
            snprintf(full_path, sizeof(full_path), "%s/%s", path, entries[i]->d_name);

            struct stat fileStat;
            if(stat(full_path, &fileStat) == 0) {
                totalBlocks += fileStat.st_blocks; // st_blocks gives the number of 512-byte blocks allocated
            }
        }
        printf("total %ld\n", totalBlocks / 2);
    }
    for(int i = 0; i < numEntries; i++){
        if(!showAll && entries[i]->d_name[0] == '.'){
            free(entries[i]);
            continue;
        }

        char full_path[MAX_LENGTH];
        snprintf(full_path, sizeof(full_path), "%s/%s", path, entries[i]->d_name);
        
        if(showDetails){
            struct stat fileStat;
            if(stat(full_path, &fileStat) == 0){
                printf((S_ISDIR(fileStat.st_mode)) ? "d" : "-");
                printf((fileStat.st_mode & S_IRUSR) ? "r" : "-");
                printf((fileStat.st_mode & S_IWUSR) ? "w" : "-");
                printf((fileStat.st_mode & S_IXUSR) ? "x" : "-");
                printf((fileStat.st_mode & S_IRGRP) ? "r" : "-");
                printf((fileStat.st_mode & S_IWGRP) ? "w" : "-");
                printf((fileStat.st_mode & S_IXGRP) ? "x" : "-");
                printf((fileStat.st_mode & S_IROTH) ? "r" : "-");
                printf((fileStat.st_mode & S_IWOTH) ? "w" : "-");
                printf((fileStat.st_mode & S_IXOTH) ? "x" : "-");

                printf(" %ld ", fileStat.st_nlink);

                struct passwd *pw = getpwuid(fileStat.st_uid);
                if(pw != NULL){
                    printf("%s ", pw->pw_name);
                } 
                else{
                    printf("%d ", fileStat.st_uid);
                }

                struct group *gr = getgrgid(fileStat.st_gid);
                if(gr != NULL){
                    printf("%s ", gr->gr_name);
                } 
                else{
                    printf("%d ", fileStat.st_gid);
                }
                printf("%ld ", fileStat.st_size);

                struct tm *timeinfo = localtime(&fileStat.st_mtime);
                char timeStr[20];
                strftime(timeStr, sizeof(timeStr), "%b %d %H:%M", timeinfo);
                printf("%s ", timeStr);
            }
        }
        printColoredFileType(entries[i], path);
        free(entries[i]);
    }
    free(entries);
    closedir(dir);
}
