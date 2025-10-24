#include "headers.h"
pid_t current_fg_pid = -1;
char command_fgbg_global[MAX_LENGTH] = "";
int main()
{
    char input[MAX_LENGTH];
    char temp_input[MAX_LENGTH];
    char home_directory[MAX_LENGTH];
    getcwd(home_directory,sizeof(home_directory)); 
    char cwd[MAX_LENGTH]; 
    char prev_directory[MAX_LENGTH] = "";
    snprintf(commands_file, sizeof(commands_file), "%s/%s", home_directory, FILENAME);
    char* commands[MAX_NO_COMMANDS];
    int command_count = 0;
    load_file(commands, &command_count);
    char ext_prompt[MAX_LENGTH + 10] = "";
    char* bg_commands[MAX_LENGTH];
    int skip_prompt = 0;
    signal(SIGINT, ctrl_c_handler);
    // signal(SIGTSTP, ctrl_z_handler);
    while(1)
    {
        setup_sigchld_handler();
        // check_background_processes(bg_commands, bg_count);
        getcwd(cwd,sizeof(cwd));
        if(skip_prompt == 0){
            printPrompt(cwd, home_directory, ext_prompt);
            // fgets(input, 4096, stdin);
            if (fgets(input, MAX_LENGTH, stdin) == NULL) {
                if (feof(stdin)) {
                    ctrl_d_handler();
                    break;
                } else {
                    perror("\033[0;31mfgets\033[0m");
                    continue;
                }
            }
            input[strcspn(input, "\n")] = '\0';
        }
        strcpy(temp_input, input);
        strcpy(command_fgbg_global, input);
        char* delimiters = ";&";
        char* delimiters2 = " \t";
        char* fgbg_token = strtok(input, delimiters);
        char* commands_fgbg[MAX_LENGTH] = {NULL};
        int is_background[MAX_LENGTH] = {0};
        int command_fgbg_count = 0;
        int len_in = 0;
        int indiv_bg = 0;
        skip_prompt = 0;
        while(fgbg_token != NULL && command_fgbg_count < MAX_LENGTH){
            commands_fgbg[command_fgbg_count] = fgbg_token;
            len_in += strlen(fgbg_token);
            char next_char = temp_input[len_in + indiv_bg];
            if(next_char == '&'){
                is_background[command_fgbg_count] = 1;  // Mark as background process
                indiv_bg++;
            } else {
                is_background[command_fgbg_count] = 0;  // Mark as foreground process
            }
            command_fgbg_count++;
            fgbg_token = strtok(NULL, delimiters);
        }
        strcpy(ext_prompt, "");
        int elapsed_time = 0;
        time_t start, end;
        time(&start);
        for(int j = 0; j < command_fgbg_count; j++){  
            char individual_comm[MAX_LENGTH];
            strcpy(individual_comm, commands_fgbg[j]);
            int background = is_background[j];
            char* args[64] = {NULL};
            char dup_command_fgbg[MAX_LENGTH];
            strcpy(dup_command_fgbg, commands_fgbg[j]);
            char* token = strtok(dup_command_fgbg, " \t");
            int i = 0;
            int custom = 0;
            while (token != NULL && i < 64) {
                args[i] = token;
                token = strtok(NULL, " \t");
                i++;
            }
            if(strcmp(args[0], "hop") == 0 || strcmp(args[0], "reveal") == 0 || strcmp(args[0], "log") == 0 || strcmp(args[0], "seek") == 0 || strcmp(args[0], "proclore") == 0 || strcmp(args[0], "activities") == 0 || strcmp(args[0], "neonate") == 0 || strcmp(args[0], "ping") == 0 ||strcmp(args[0], "iMan") == 0){
                custom = 1;              
                // if(strchr(commands_fgbg[j], '|') != NULL) {
                //     execute_piped_commands(commands_fgbg[i], home_directory, cwd, prev_directory, input, commands, &command_count, commands_fgbg, &command_fgbg_count, is_background, bg_commands, &bg_count, j, len_in, bgProcessList, background, elapsed_time, ext_prompt, individual_comm, custom, &skip_prompt);
                // }
                // else{
                    execute_custom_commands(commands_fgbg[j], home_directory, cwd, prev_directory, input, commands, &command_count, commands_fgbg, &command_fgbg_count, is_background, j, len_in, bgProcessList, &bg_count, &skip_prompt);
                // }
                continue;
            }
            // Check if the command contains pipes (|)
            if (strchr(commands_fgbg[j], '|') != NULL) {
                // Handle piped commands
                if (fork() == 0) {
                    execute_piped_commands(commands_fgbg[j], home_directory, cwd, prev_directory, input, commands, &command_count, commands_fgbg, &command_fgbg_count, is_background, bg_commands, &bg_count, j, len_in, bgProcessList, background, elapsed_time, ext_prompt, individual_comm, custom, &skip_prompt);
                } else {
                    wait(NULL);
                }
            } else {
                // Handle single command with possible redirection
                if (fork() == 0) {
                    execute_system_commands(commands_fgbg[j], bg_commands, &bg_count, bgProcessList, background, elapsed_time, ext_prompt, individual_comm);
                } else {
                    wait(NULL);
                }
            }
        }
        time(&end);
        elapsed_time = (int)difftime(end, start);
        if(elapsed_time > 2){
            // char* elapsed_commands[1024];
            // char* command;
            // char* time_comm;
            // char concat[4000];
            // for(int i = 0; i < command_fgbg_count && is_background[i] != 1; i++){
            //     command = strtok(input, " "); // Tokenize by space
            //     if (command != NULL) {
            //         // Use strtok to extract the second token (argument)
            //         time_comm = strtok(NULL, " "); // Get the next part
            //     }
            //     sprintf(elapsed_commands[i]," %s : %ds", command, atoi(time_comm));
            //     printf("%s\n", elapsed_commands[i]);
            //     strcat(concat, elapsed_commands[i]);
            //     printf("%s\n", concat);
            // }
            sprintf(ext_prompt," sleep : %ds", elapsed_time);
        }
        else{
            strcpy(ext_prompt, "");
        }
        if(!strstr(temp_input, "log")){
            add_to_file(temp_input, commands, &command_count);
        }
    }

    return 0;
}
