#include "headers.h"

char commands_file[4200]; 

void add_to_file(char* command, char* commands[], int* count)
{
    if(command == NULL || strlen(command) == 0){
        return;  // Return early if command is invalid
    }

    int flag_add = 1;
    if((*count) > 0 && strcmp(command, commands[(*count) - 1]) == 0){
        flag_add = 0; // Avoid adding consecutive duplicate commands
    }

    if(flag_add == 1){
        if((*count) >= MAX_NO_COMMANDS){
            // Shift commands if the array is full
            free(commands[0]); // Free memory of the oldest command
            for(int i = 1; i < (*count); i++){
                commands[i - 1] = commands[i];
            }
            (*count) = MAX_NO_COMMANDS - 1; // Adjust count to max limit
        }
        commands[(*count)] = strdup(command); // Duplicate the command
        (*count)++; // Increment count
    }

    // Open file after validation
    int fd = open(commands_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if(fd == -1)
    {
        perror("\033[0;31mError opening history file\033[0m");
        return;
    }

    // Rewrite the entire file with the updated commands
    for(int i = 0; i < (*count); i++){
        write(fd, commands[i], strlen(commands[i]));
        write(fd, "\n", 1);
    }

    close(fd);
}

void clear_file(char* commands[], int* count)
{
    int fd = open(commands_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if(fd == -1)
    {
        perror("\033[0;31mError opening history file\033[0m");
        return;
    }
    // Clear file contents
    ftruncate(fd, 0);
    for(int i = 0; i < (*count); i++)
    {
        free((void *)commands[i]);
    }
    *count = 0;
    close(fd);
}

void print_file(char* commands[], int count)
{
    for(int i = 0; i < count; i++)
    {
        printf("%s\n", commands[i]);
    }
}

void load_file(char* commands[], int* count)
{
    int fd = open(commands_file, O_RDONLY);
    if(fd == -1)
    {
        perror("\033[0;31mError opening history file\033[0m");
        return;
    }

    char buffer[MAX_LENGTH];
    ssize_t read_bytes;
    char incomplete_command[MAX_LENGTH] = ""; // To handle command that might span reads

    while ((read_bytes = read(fd, buffer, sizeof(buffer) - 1)) > 0)
    {
        buffer[read_bytes] = '\0'; // Null-terminate the read data

        // If there is an incomplete command from the last read, prepend it
        char command_line[8192];  // To handle large concatenated buffer
        strcpy(command_line, incomplete_command);
        strcat(command_line, buffer);

        // Clear the incomplete command buffer
        incomplete_command[0] = '\0';

        // Tokenize the buffer based on newlines
        char *token = strtok(command_line, "\n");

        while (token != NULL)
        {
            // If we're out of space, store the last partial command and break
            if (*count >= MAX_NO_COMMANDS) {
                strncpy(incomplete_command, token, sizeof(incomplete_command) - 1);
                incomplete_command[sizeof(incomplete_command) - 1] = '\0'; // Ensure null-terminated
                break;
            }

            // Duplicate the command and add it to the array
            commands[(*count)++] = strdup(token);

            token = strtok(NULL, "\n");
        }

        // Handle the case where the last command is incomplete (spanning reads)
        if (token == NULL && buffer[read_bytes - 1] != '\n') {
            // Store the remaining part as an incomplete command
            strncpy(incomplete_command, command_line + strlen(command_line) - strlen(token), sizeof(incomplete_command) - 1);
            incomplete_command[sizeof(incomplete_command) - 1] = '\0';
        }
    }

    close(fd);
}
