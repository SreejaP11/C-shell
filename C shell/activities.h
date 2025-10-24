#ifndef __ACTIVITIES_H
#define __ACTIVITIES_H

void addProcess(pid_t pid, const char *command, const char *state);
void updateProcessState(pid_t pid, const char *state); 
int compareProcesses(const void *a, const void *b);
void displayActivities();

#endif