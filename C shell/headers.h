#ifndef HEADERS_H_
#define HEADERS_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <pwd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <grp.h>
#include <stdbool.h>
#include <sys/wait.h> 
#include <signal.h> 
#include <ctype.h>
#include <termios.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define MAX_COMMANDS 64
#define MAX_LENGTH 4096
#define MAX_BG_PROCESSES 256
#define BUFFER_SIZE 256
typedef struct {
    pid_t pid;
    char command[MAX_LENGTH];
    char state[10];
} BgProcess;
extern int bg_count;
extern BgProcess bgProcessList[MAX_BG_PROCESSES];
extern pid_t current_fg_pid;
extern char command_fgbg_global[MAX_LENGTH];
#include "prompt.h"
#include "hop.h"
#include "reveal.h"
#include "proclore.h"
#include "log.h"
#include "fgbg.h"
#include "seek.h"
#include "activities.h"
#include "commands.h"
#include "io_and_pipe.h"
#include "iMan.h"
#include "neonate.h"
#include "ping.h"

#endif