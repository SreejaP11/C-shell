#include "headers.h"

void execute_command(char **args, int background, int* elapsed_time, char* ext_prompt, char* command, BgProcess bgProcessList[], int* bg_count) 
{
    pid_t pid = fork();
    
    if (pid == 0) {
        handle_redirection(args);
        // Child process
        if (execvp(args[0], args) == -1) {
            fprintf(stderr, "\033[0;31mERROR: '%s' is not a valid command\033[0m\n", args[0]);
            exit(EXIT_FAILURE);
        }
    } else if (pid > 0) {
        // Parent process
        if (background) {
            printf("[%d] : %d\n", (*bg_count) + 1, pid);
            addProcess(pid, command, "Running");
        } else {
            int status;
            waitpid(pid, &status, 0);
        }
    } else {
        // Fork failed
        perror("\033[0;31mERROR");
    }
}
 
void sigchld_handler(int sig) 
{
    pid_t pid;
    int status;

    // Use waitpid to reap the terminated background process
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        if (WIFEXITED(status)) {
            printf("\n[Process %d exited normally with status %d]\n", pid, WEXITSTATUS(status));
            updateProcessState(pid, "Stopped");
        } else if (WIFSIGNALED(status)) {
            printf("\n[Process %d exited abnormally with signal %d]\n", pid, WTERMSIG(status));
            updateProcessState(pid, "Stopped");        
        }
        fflush(stdout);
    }
    // Handle errors from waitpid
    if (pid == -1 && errno != ECHILD) {
        perror("\033[0;31mwaitpid\033[0m");
    }
}

void setup_sigchld_handler() 
{
    struct sigaction sa;
    sa.sa_handler = sigchld_handler;  // Set our custom handler
    sigemptyset(&sa.sa_mask);         // Block no other signals during the handler
    sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;  // Restart interrupted system calls and don't catch stopped children
    
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("\033[0;31msigaction\033[0m");
        exit(1);
    }
}

// void check_background_processes(char* bg_commands[], int bg_count) 
// {
//     int status;
//     pid_t pid;
//     for(int i = 0; i < bg_count; i++){
//         while((pid = waitpid(-1, &status, WNOHANG)) > 0){
//             if(WIFEXITED(status)){
//                 printf("%s with pid %d exited normally\n", bg_commands[i], pid);
//             } else if (WIFSIGNALED(status)) {
//                 printf("%s with pid %d exited abnormally\n", bg_commands[i], pid);
//             }
//         }
//     }
// }