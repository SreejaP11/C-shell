#include "headers.h"

void reset_redirection(int saved_stdout, int saved_stdin) 
{
    dup2(saved_stdout, STDOUT_FILENO);
    dup2(saved_stdin, STDIN_FILENO);
    close(saved_stdout);
    close(saved_stdin);
}

void execute_custom_commands(char* command_fgbg, char* home_directory, char* cwd, char* prev_directory, char* input, char** commands, int* command_count, char** commands_fgbg, int* command_fgbg_count, int* is_background, int j, int len_in, BgProcess bgProcessList[], int* bg_count, int* skip_prompt)
{
    char dup_command_fgbg[MAX_LENGTH];
    strcpy(dup_command_fgbg, command_fgbg);
    char* token = strtok(command_fgbg, " \t");
    char* args[64] = {NULL};
    int i = 0;
    while (token != NULL && i < 64) {
        args[i] = token;
        token = strtok(NULL, " \t");
        i++;
    }
    int saved_stdout = dup(STDOUT_FILENO);
    int saved_stdin = dup(STDIN_FILENO);
    handle_redirection(args);
    if(strcmp(args[0],"hop")==0){
        int i=1;
        if(args[1] == NULL){
            chdir(home_directory);
            strcpy(prev_directory, cwd);
        }
        while(args[i] != NULL){
            if(strcmp(args[i], "~") != 0 && strcmp(args[i], "-") != 0 && args[1][0] != '~' && args[1][0] != '-'){
                if(is_directory(args[i]) == 0){
                    break;
                }
            }
            hop(args[i],cwd,prev_directory, home_directory);
            strcpy(prev_directory,cwd); 
            i++;
        }
    }
    else if(strcmp(args[0],"reveal")==0) {
        bool showAll = false;
        bool showDetails = false;
        char path[MAX_LENGTH];
        int hyphen_count = 0;
        for(int i=1;i<64;i++){
            if(args[i]!=NULL && args[i][0]=='-' && args[i][1] != '/'){
                hyphen_count++;
                for(int j=1;args[i][j]!='\0';j++){
                    if(args[i][j]=='a'){
                        showAll=true;
                    }
                    if(args[i][j]=='l'){
                        showDetails=true;
                    }
                }
            }
        }
        if(args[hyphen_count + 1] != NULL){
            strcpy(path, args[hyphen_count + 1]);
            if(strcmp(path, "~") == 0){
                strcpy(path, home_directory);
            }
            else if(path[0] == '~' && path[1] == '/'){
                char next_path[2048];
                strcpy(next_path, path + 2);
                char respective_path[MAX_LENGTH];
                snprintf(respective_path, sizeof(respective_path), "%s/%s", home_directory, next_path);
                strcpy(path, respective_path);
            }
            else if(path[0] == '-' && path[1] == '/'){
                char next_path[2048];
                strcpy(next_path, path + 2);
                char respective_path[MAX_LENGTH];
                snprintf(respective_path, sizeof(respective_path), "%s/%s", prev_directory, next_path);
                strcpy(path, respective_path);
            }
        } 
        else if(strcmp(args[hyphen_count], "-") == 0){
            if(strcmp(prev_directory, "") == 0){
                printf("\033[0;31mERROR: Previous directory (OLDPWD) is not set\033[0m\n");
                reset_redirection(saved_stdout, saved_stdin);
                return;
            }
            strcpy(path, prev_directory);
        }
        else{
            strcpy(path, cwd);
        }

        if(is_directory(path) != 0){
            reveal(showAll, showDetails, path, home_directory);        
        }
        // else{
        //     valid_command = 0;
        // }
    }
    else if(strcmp(args[0],"proclore")==0){
        if(args[1]==NULL){
            printProcInfo(getpid());
        } 
        else{
            pid_t pid=atoi(args[1]);
            if(pid>0){
                printProcInfo(pid);
            } 
            else{
                printf("Invalid PID\n");
            }
        }
    }
    else if(strcmp(args[0], "log") == 0 && args[1] == NULL){
        print_file(commands, (*command_count));
    }
    else if(strcmp(args[0], "log") == 0 && strcmp(args[1], "purge") == 0){
        clear_file(commands, command_count);
    }
    else if(strcmp(args[0], "log") == 0 && strcmp(args[1], "execute") == 0){
        int index = atoi(args[2]);
        if(index > (*command_count)){
            printf("\033[0;31mInvalid index\033[0m\n");
        }
        (*skip_prompt) = 1;
        strcpy(input, commands[(*command_count) - index]);
        reset_redirection(saved_stdout, saved_stdin);
        return;
    }
    else if(strcmp(args[0], "log") == 0 && args[1] != NULL){
        if(strcmp(args[1], "purge") != 0 && strcmp(args[1], "execute") != 0){
            fprintf(stderr, "\033[0;31mERROR: '%s' is not a valid command\033[0m\n", dup_command_fgbg);
        }
    }
    else if(strcmp(args[0], "seek") == 0){
        int find_files = 1;
        int find_dirs = 1;
        int execute_flag = 0;
        char search_target[MAX_LENGTH];
        char target_dir[MAX_LENGTH]; // Default to current directory
        char main_target_dir[MAX_LENGTH];
        int match_count = 0;
        char matched_path[MAX_LENGTH];
        int invalid = 0;
        int error;
        if(args[1] == NULL){
            fprintf(stderr, "\033[0;31mUsage: seek <flags> <search> <target_directory>\033[0m\n");
            return;
        }
        int hyphen_count_2 = 0;
        while(hyphen_count_2 < 3 && args[hyphen_count_2 + 1][0] == '-' && invalid == 0){
            if(strcmp(args[hyphen_count_2 + 1], "-d") == 0){
                find_files = 0;
            } 
            else if(strcmp(args[hyphen_count_2 + 1], "-f") == 0){
                find_dirs = 0;
            } 
            else if(strcmp(args[hyphen_count_2 + 1], "-e") == 0){
                execute_flag = 1;
            } 
            else{
                fprintf(stderr, "\033[0;31mInvalid flags!\033[0m\n");
                invalid = 1;
                return;
            }
            hyphen_count_2++;
        }
        if(invalid == 1){
            return;
        }
        if(find_files == 0 && find_dirs == 0){
            fprintf(stderr, "\033[0;31mInvalid flags!\033[0m\n");
            return;
        }
        if(args[hyphen_count_2 + 1] != NULL){
            strcpy(search_target, args[hyphen_count_2 + 1]);
        } 
        else{
            fprintf(stderr, "\033[0;31mNo search target provided!\033[0m\n");
            return;
        }
        if(args[hyphen_count_2 + 2] != NULL){
            strcpy(target_dir, args[hyphen_count_2 + 2]);
            strcpy(main_target_dir, target_dir);
            if(strcmp(target_dir, "~") == 0){
                strcpy(target_dir, home_directory);
                strcpy(main_target_dir, home_directory);
            }
            else if(strcmp(target_dir, "-") == 0){
                strcpy(target_dir, prev_directory);
                strcpy(main_target_dir, prev_directory);
            }
        }
        else{
            strcpy(target_dir, cwd);
            strcpy(main_target_dir, cwd);
        }
        error = seek_directory(search_target, target_dir, find_files, find_dirs, execute_flag, main_target_dir, home_directory, prev_directory, &match_count, matched_path);
        if(match_count == 0 && error != 1){
            printf("No match found!\n");
            reset_redirection(saved_stdout, saved_stdin);
            return;
        }
        if(execute_flag && match_count > 1){
            printf("Multiple matches found; ignoring -e flag\n");
        }
        else if(match_count == 1 && execute_flag == 1){
            execute(matched_path, main_target_dir, find_dirs, find_files);
        }
    }
    else if(strcmp(args[0], "activities") == 0 && args[1] == NULL){
        displayActivities();
    }
    else if(strcmp(args[0], "neonate") == 0){
        if(args[3] != NULL || strcmp(args[1], "-n") != 0) {
            fprintf(stderr, "Usage: %s -n [time_arg]\n", args[0]);
            return;
        }

        int time_arg = atoi(args[2]);
        if (time_arg < 0) {
            fprintf(stderr, "Invalid time argument.\n");
            return;
        }

        disable_buffering();
        while (1) {
            int most_recent_pid = get_most_recent_pid();
            if (most_recent_pid != -1) {
                printf("%d\n", most_recent_pid);
            } else {
                fprintf(stderr, "Failed to get the most recent PID.\n");
            }
            sleep(time_arg);
            if (is_x_pressed()) {
                restore_buffering();
                reset_redirection(saved_stdout, saved_stdin);
                return;
            }
        }
    }
    else if (strncmp(args[0], "ping", 4) == 0) {
        int pid, signal_number;
        if(sscanf(dup_command_fgbg, "ping %d %d", &pid, &signal_number) == 2) {
            ping_command(pid, signal_number);
        } else {
            printf("Invalid ping command format\n");
        }
    }
    else if(strcmp(args[0], "iMan") == 0){
        fetch_man_page(args[1]);
    }
    reset_redirection(saved_stdout, saved_stdin);
}

void execute_system_commands(char* command_fgbg, char** bg_commands, int* bg_count, BgProcess bgProcessList[], int background, int elapsed_time, char* ext_prompt, char* individual_comm)
{
    char* token = strtok(command_fgbg, " \t");
    char* args[64] = {NULL};
    int i = 0;
    
    while (token != NULL && i < 64) {
        args[i] = token;
        token = strtok(NULL, " \t");
        i++;
    }
    execute_command(args, background, &elapsed_time, ext_prompt, individual_comm, bgProcessList, bg_count);
    if(background == 1){
        (*bg_count)++;
        bg_commands[(*bg_count) - 1] = malloc(strlen(args[0]) + 1);
        if(bg_commands[(*bg_count) - 1] == NULL){
            perror("\033[0;31mmalloc failed\033[0m");
            exit(EXIT_FAILURE);
        }
        strcpy(bg_commands[(*bg_count) - 1], args[0]);
    }
}
