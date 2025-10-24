#include "headers.h"

// Function to disable terminal buffering for 'x' key detection
void disable_buffering() 
{
    struct termios tty;
    tcgetattr(STDIN_FILENO, &tty);
    tty.c_lflag &= ~ICANON;  // Disable canonical mode
    tty.c_lflag &= ~ECHO;    // Disable echoing
    tcsetattr(STDIN_FILENO, TCSANOW, &tty);
}

// Function to restore terminal settings
void restore_buffering() 
{
    struct termios tty;
    tcgetattr(STDIN_FILENO, &tty);
    tty.c_lflag |= ICANON;  // Enable canonical mode
    tty.c_lflag |= ECHO;    // Enable echoing
    tcsetattr(STDIN_FILENO, TCSANOW, &tty);
}

// Function to detect 'x' keypress non-blocking
int is_x_pressed() 
{
    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);

    char buf = 0;
    int n = read(STDIN_FILENO, &buf, 1);
    
    fcntl(STDIN_FILENO, F_SETFL, flags);  // Restore original flags

    if (n > 0 && buf == 'x') {
        return 1;
    }
    return 0;
}

// Function to get the most recent process PID from /proc/loadavg
int get_most_recent_pid() 
{
    FILE *file = fopen("/proc/loadavg", "r");
    if (file == NULL) {
        perror("\033[0;31mFailed to open /proc/loadavg\033[0m");
        return -1;
    }

    // Reading the content of /proc/loadavg
    char buffer[BUFFER_SIZE];
    if (fgets(buffer, sizeof(buffer), file) != NULL) {
        // The last field in /proc/loadavg is the most recent PID
        char *pid_str = strrchr(buffer, ' ');
        if (pid_str != NULL) {
            fclose(file);
            return atoi(pid_str + 1);  // Convert the PID string to an integer
        }
    }
    fclose(file);
    return -1;
}
