#include "headers.h"

// Function to check and handle input/output redirection
void handle_redirection(char **args) 
{
    int i = 0;
    while (args[i] != NULL) {
        if (strcmp(args[i], "<") == 0) {
            // Input redirection
            int fd_in = open(args[i + 1], O_RDONLY);
            if (fd_in < 0) {
                perror("\033[0;31mNo such input file found!\033[0m");
                exit(1);
            }
            dup2(fd_in, STDIN_FILENO);
            close(fd_in);
            args[i] = NULL; // Remove the redirection from the args list
        } else if (strcmp(args[i], ">") == 0) {
            // Output redirection (overwrite)
            int fd_out = open(args[i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd_out < 0) {
                perror("\033[0;31mError opening output file\033[0m");
                exit(1);
            }
            dup2(fd_out, STDOUT_FILENO);
            close(fd_out);
            args[i] = NULL;
        } else if (strcmp(args[i], ">>") == 0) {
            // Output redirection (append)
            int fd_out = open(args[i + 1], O_WRONLY | O_CREAT | O_APPEND, 0644);
            if (fd_out < 0) {
                perror("\033[0;31mError opening output file\033[0m");
                exit(1);
            }
            dup2(fd_out, STDOUT_FILENO);
            close(fd_out);
            args[i] = NULL;
        }
        i++;
    }
}

void execute_piped_commands(char* command_fgbg, char* home_directory, char* cwd, char* prev_directory, char* input, char** commands, int* command_count, char** commands_fgbg, int* command_fgbg_count, int* is_background, char** bg_commands, int* bg_count, int j, int len_in, BgProcess bgProcessList[], int background, int elapsed_time, char* ext_prompt, char* individual_comm, int custom, int* skip_prompt) 
{
    char dup_command_fgbg[MAX_LENGTH];
    strcpy(dup_command_fgbg, command_fgbg);

    // Trim leading/trailing spaces to ensure clean command parsing
    while (*command_fgbg == ' ') command_fgbg++;
    char* end = command_fgbg + strlen(command_fgbg) - 1;
    while (end > command_fgbg && *end == ' ') end--;
    *(end + 1) = '\0';

    // Error 1: Check if the command starts with a pipe
    if (command_fgbg[0] == '|') {
        printf("\033[0;31mInvalid use of pipe\033[0m\n");
        return;
    }

    // Error 2: Check if there's a pipe at the end or a command with no argument between pipes
    if (command_fgbg[strlen(command_fgbg) - 1] == '|') {
        printf("\033[0;31mInvalid use of pipe\033[0m\n");
        return;
    }

    // Split command by pipe symbols
    char* pipe_commands[MAX_COMMANDS];
    int num_commands = 0;
    char* pipe_token = strtok(command_fgbg, "|");

    // Iterate through commands separated by pipes
    while (pipe_token != NULL) {
        // Trim leading and trailing spaces of each token
        while (*pipe_token == ' ') pipe_token++;
        char* end = pipe_token + strlen(pipe_token) - 1;
        while (end > pipe_token && *end == ' ') end--;
        *(end + 1) = '\0';

        // Check for empty commands (no command between pipes or after pipes)
        if (strlen(pipe_token) == 0) {
            printf("\033[0;31mInvalid use of pipe\033[0m\n");
            return;
        }

        // Error 3: Check for cases where '>' follows a pipe without a command
        if(pipe_token[0] == '>' || pipe_token[0] == '<' || (pipe_token[0] == '>' && pipe_token[1] == '>')) {
            printf("\033[0;31mERROR: No command entered before redirection\033[0m\n");
            return;
        }

        // Store the valid pipe command
        pipe_commands[num_commands] = pipe_token;
        num_commands++;
        pipe_token = strtok(NULL, "|");
    }

    pipe_commands[num_commands] = NULL;  // Mark end of command list

    int pipe_fd[2];
    int prev_fd = -1;  // For the input of the current command (if it is piped)
    pid_t pids[MAX_COMMANDS];  // Store child process IDs
    for (int i = 0; i < num_commands; i++) {
        if (pipe(pipe_fd) == -1) {
            perror("\033[0;31mpipe\033[0m");
            exit(1);
        }

        pids[i] = fork();
        if (pids[i] == 0) {
            // Child process

            if (i > 0) {
                // If it's not the first command, read from the previous pipe
                dup2(prev_fd, STDIN_FILENO);
                close(prev_fd);
            }

            if (i < num_commands - 1) {
                // If it's not the last command, pipe the output to the next command
                dup2(pipe_fd[1], STDOUT_FILENO);
            }
            close(pipe_fd[0]);  // Close unused read end of the current pipe
            close(pipe_fd[1]);  // Close unused write end of the current pipe

            // Execute the command
            if(custom == 1){
                execute_custom_commands(pipe_commands[i], home_directory, cwd, prev_directory, input, commands, command_count, commands_fgbg, command_fgbg_count, is_background, j, len_in, bgProcessList, bg_count, skip_prompt);
            }
            else{
                execute_system_commands(pipe_commands[i], bg_commands, bg_count, bgProcessList, background, elapsed_time, ext_prompt, individual_comm);
            }
            exit(0);  // Ensure child exits after execution
        } else {
            // Parent process
            close(pipe_fd[1]);  // Close the write end of the pipe
            prev_fd = pipe_fd[0];  // Save the read end for the next command
        }
    }

    // After the loop, wait for all child processes
    for (int i = 0; i < num_commands; i++) {
        waitpid(pids[i], NULL, 0);
    }
}