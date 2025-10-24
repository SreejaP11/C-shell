#include "headers.h"

void ctrl_c_handler(int sig) 
{
    if(current_fg_pid != -1) {
        kill(current_fg_pid, SIGINT);
        current_fg_pid = -1;
    }
}

// void ctrl_z_handler(int sig) 
// {
//     if (current_fg_pid != -1) {
//         kill(current_fg_pid, SIGTSTP);
//         addProcess(current_fg_pid, command_fgbg_global, "Stopped", bgProcessList, &bg_count);
//         // updateProcessState(current_fg_pid, "Stopped"); // mark as stopped
//         current_fg_pid = -1;
//     }
// }

void ctrl_d_handler() 
{
    // Terminate the foreground process if it exists
    if (current_fg_pid > 0) 
    {
        kill(current_fg_pid, SIGTERM);
    }

    // Terminate all background processes
    for (int i = 0; i < bg_count; i++) {
        kill(bgProcessList[i].pid, SIGTERM);
    }

    // Exit the shell
    exit(0);
}

void remove_process(int pid) 
{
    for (int i = 0; i < bg_count; i++) {
        if (bgProcessList[i].pid == pid) {
            for (int j = i; j < bg_count - 1; j++) {
                bgProcessList[j] = bgProcessList[j + 1];
            }
            bg_count--;
            return;
        }
    }
}

void ping_command(int pid, int signal_number)
{
    signal_number %= 32;
    if (kill(pid, 0) == -1 && errno == ESRCH) {
        printf("No such process found\n");
    } else {
        if (kill(pid, signal_number) == 0) {
            printf("Sent signal %d to process with pid %d\n", signal_number, pid);
            if (signal_number == SIGKILL){
                remove_process(pid); // remove if terminated
            }
            else if(signal_number == SIGTERM) {
                updateProcessState(pid, "Stopped");
            }
        } else {
            perror("\033[0;31mError sending signal\033[0m");
        }
    }
}