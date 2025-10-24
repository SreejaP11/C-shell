#include "headers.h"
int bg_count = 0;  // This should be in a .c file, not a header file
BgProcess bgProcessList[MAX_BG_PROCESSES];
void addProcess(pid_t pid, const char *command, const char *state) 
{
    bgProcessList[bg_count].pid = pid;
    strncpy(bgProcessList[bg_count].command, command, MAX_LENGTH);
    strncpy(bgProcessList[bg_count].state, state, 9);
}

void updateProcessState(pid_t pid, const char *state) 
{
    for (int i = 0; i < bg_count; i++) {
        if (bgProcessList[i].pid == pid) {
            strncpy(bgProcessList[i].state, state, 9);
            break;
        }
    }
}

int compareProcesses(const void *a, const void *b) 
{
    BgProcess *procA = (BgProcess *)a;
    BgProcess *procB = (BgProcess *)b;
    if (procA->pid < procB->pid) {
        return -1;  // procA should come before procB
    } else if (procA->pid > procB->pid) {
        return 1;   // procA should come after procB
    } else {
        return 0;   // both are equal
    }
}

void displayActivities() 
{
    // Sort the process list lexicographically by command name
    qsort(bgProcessList, bg_count, sizeof(BgProcess), compareProcesses);

    // Display the list of processes
    for (int i = 0; i < bg_count; i++) {
        printf("%d : %s - %s\n", bgProcessList[i].pid, bgProcessList[i].command, bgProcessList[i].state);
    }
}
