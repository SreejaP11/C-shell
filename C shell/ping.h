#ifndef __PING_H
#define __PING_H

void ctrl_c_handler(int sig);
void ctrl_d_handler();
// void ctrl_z_handler(int sig);
void remove_process(int pid);
void ping_command(int pid, int signal_number);

#endif