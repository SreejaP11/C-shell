# Custom C shell implementation

# Instructions to run

- To compile and run the code

      make
      ./a.out

- All the header files are included in "headers.h".
- Main header file consists of some macros, structs(BgProcess) and extern variables.
- All the specification's tasks are called through main funnction 'main.c'.
- The code in main.c is split into two functions in <b>commands.c</b> for separating custom commands and system commands(execute_custom_commands and execute_system_commands respectively).
## Specification 1 : Display Requirement
- The corresponding files are <b>prompt.h</b> and <b>prompt.c</b>
- Implemented a shell prompt that dynamically displays the current user's username, the system's hostname, and the current working directory path. 
- The prompt automatically updates to reflect changes in the directory path as the user navigates through the file system.
- The directory from which the shell is invoked becomes the home directory for the shell. All paths within this directory should be shown relative to it(denoted by '~'), while paths outside the home directory are shown as absolute paths.
- #### System calls 
    - getlogin() is used to fetch the username.
    - gethostname() retrieves the system's hostname.
-  The prompt is colorized using ANSI escape code of purple colour ('\033[0;35m').

## Specification 2 & 6 : Input Requirements & System commands
- The corresponding files are <b>fgbg.h</b> and <b>fgbg.c</b>
- Implementation of foreground and background execution of commands.
- Input is tokenized using strtok with delimiters ";&" for command separation and " \t" for argument separation.
- Commands are processed to determine if they should be executed in the background (if they end with &).
- Commands are executed using fork and execvp.
- The fork function creates a new process, and execvp replaces the child process's memory space with the new command.
- The parent process waits for the child process to complete if it's a foreground process.
- Background processes are started by appending & and the PID is printed.
- A list of background commands (bg_commands assumed 256 is the maximum number) is maintained.
- Error commands are also taken into consideration and raises and error.
- The code for timing of commands(elapsed time > 2 sec) is present in the main.c file and here it is assumed to be sleep process only.
- The execute_command function handles the execution, and it checks whether a process should be handled in the foreground or background.
Part - A
- The check_background_processes function uses waitpid with WNOHANG to periodically check if any background processes have finished.
- Messages are printed when background processes complete, after user interaction(entering the next command).
- The status of the completed background processes are printed after entering a command and execution of it.

Part - B
- The code in setup_sigchld_handler and sigchld_handler functions defines a signal handler for SIGCHLD to manage terminated background processes. 
- The sigchld_handler function uses waitpid to reap child processes that have exited without blocking the parent. It checks if the child exited normally or was terminated by a signal and prints a corresponding message(as soon as it terminates), updating the process state. Errors from waitpid are also handled.
- The setup_sigchld_handler function configures this signal handler using sigaction. It ensures that the SIGCHLD signal is handled without interrupting system calls (SA_RESTART) and that terminated but not stopped child processes are caught (SA_NOCLDSTOP).
- It is not working completely fine for multiple background processes and also some both foreground and background inclusive processes(displaying the exit message). Hence, activities is also affected(It works fine when only single background process commands are entered).

## Specification 3 : hop
- The corresponding files are <b>hop.h</b> and <b>hop.c</b>
- Implemented the hop command, which changes the current working directory of the shell. The directory path/name can be provided as argument to this command.
- After successfully changing the directory, the full path of the new working directory is printed to the shell.
- The hop command supports standard directory navigation flags:
    - '~' : Represents the home directory of the shell.
    - '.' : Represents the current directory.
    - '..' : Represents the parent directory.
    - '-' : Switches to the previous directory.
- The command accepts both absolute and relative paths. When multiple directory arguments are provided, hop sequentially changes to each directory from left to right.
- If anyone of the multiple directory arguments is invalid, then it hops upto the before argument and gives an erro message.
- If no argument is given, hop navigates to the home directory of the shell.
- #### System Calls:
    - chdir() is used to change the current working directory.
    - getpwuid() is used to fetch the home directory when no path is provided.
    - stat() is used to check if a given path is a valid directory before attempting to change to it.
- The 'hop' command includes error handling to manage situations where the provided path does not exist, or the previous directory is not set when using the - flag.
- Error messages are displayed in red using the ANSI escape code \033[0;31m to differentiate them from regular output.

## Specification 4 : reveal
- The corresponding files are <b>reveal.h</b> and <b>reveal.c</b>
- ‘reveal’ command lists all the files and directories in the specified directories in lexicographic order (default reveal does not show hidden files).
- Supporting flags:
    - -l : displays extra information
    - -a : displays all files, including hidden files
- Color-Coding Files and Directories:
    - Directories are printed in blue (\033[1;34m).
    - Executable files are printed in green (\033[1;32m).
    - Regular files are printed in white (default color).
- Support for Special Path Symbols along with relative and absolute paths:
    - '~' : Replaced with the user's home directory.
    - '..' : Replaced with the parent directory.
    - '-' : Replaced with the previous directory (implemented with a check to ensure it is already set using hop).
- If no argument is given, reveals at the current working directory.
- Multiple arguments will not be given as input.
- The input will always be in the format :

      reveal <flags> <path/name>
- Handles various cases of flags like -a -l, -l -a, -al, -la, -lal, -alalll, etc.
- #### System calls
    - File Management: opendir(), scandir(), closedir(), stat(), access()
    - User and Group Information: getpwuid(), getgrgid()
    - Time Handling: localtime(), strftime()
    - String Manipulation: snprintf()

## Specification 5 : log commands
- The corresponding files are <b>log.h</b> and <b>log.c</b>
- Stores up to 15 commands by default. If more than 15 commands are entered, it overwrites the oldest ones.
- Commands are stored across sessions, ensuring persistence between shell restarts.
- Commands are stored along with their arguments.
- It does not store duplicate consecutive commands.
- Commands separated by ; or & are stored in a single line.
- The log command itself is not stored. If a command string contains log (e.g., cmd1 ; log), the entire command string is not stored.
- The erroneous commands are also stored.
- Commands are stored in a file specified by the commands_file variable(commands.txt file in the home directory).
- The file is rewritten with the updated list of commands each time a new command is added.
- Commands: 
    - log: Displays the stored command history, printing from the oldest to the most recent.
    - log purge: Clears all stored commands from the log file.
    - log execute <index>: Executes the command at the specified index in the log file. The index is 1-based, with 1 being the most recent command. This command stores the executed command in the log file. (This is not completely satisfied for all types of commands(eg: log execute 1 ; pwd - the output ordering is not as expected).
- #### System calls
    - File Management: open(), write(), ftruncate(), close()
    - Reading Data: read()
    - String Manipulation: strtok()
    - Memory Management: malloc(), free()

## Specification 7 : proclore
- The corresponding files are <b>proclore.h</b> and <b>proclore.c</b>
- The proclore command is designed to retrieve and display detailed information about a process given its Process ID (PID). If no PID is provided, it displays information about the current shell process.
- Information displayed:
    - PID: The Process ID of the process.
    - Process Status: Displays the status of the process (e.g., Running, Sleeping, Zombie - R/R+/S/S+/Z).
    - Process Group: The process group ID to which the process belongs.
    - Virtual Memory: The amount of virtual memory used by the process.
    - Executable Path: The path to the executable file of the process.
- #### System calls
    - File Operations: fopen(), fgets(), fscanf(), fclose()
    - Symbolic Link Operations: readlink()
    - Process Information: getpid()
    - String Formatting: snprintf()

## Specification 8 : seek
- The corresponding files are <b>seek.h</b> and <b>seek.c</b>
- The seek command is designed to search for files and directories within a specified target directory or the current directory if none is specified. It supports flags to filter the search and performs actions based on the results.
- A call to this command will always be in the format :  
    -     seek <flags> <search> <target_directory>
- #### Search Parameters:
    - search_target: The exact name of the file or directory to search for.
    - target_directory: Directory to search in (optional; defaults to the current working directory).
- #### Flags:
    - -d: Search only for directories.
    - -f: Search only for files.
    - -e: If exactly one file or one directory is found:
        - One file: Print its contents.
        - One directory: Change to that directory.
        - If the file or directory does not have the required permissions, print an error message.
- The seek_directory function performs a recursive search through directories.
    - Directory Handling: Opens directories and reads their contents using opendir, readdir, and stat.
    - File Handling: Checks file type using S_ISDIR and S_ISREG.
    - Path Construction: Constructs relative paths from the target directory.
    - Color Coding: Files and directories are printed with specific colors (green for files, blue for directories).
- Recursive Search: The function seek_directory handles nested directories by calling itself recursively.
- Permission Checks: The 'execute'(-e flag) function checks permissions for the file or directory before performing actions.
- Output: 
    - Lists the relative paths of matching files and directories.
    - Files are displayed in green, and directories are displayed in blue.
    - If no matches are found, prints "No match found!"
    - If invalid flags are used together, prints "Invalid flags!"

## Specification 10, 11 & 12 : I/O Redirection, Pipes and Redirection along with pipes
- The corresponding files are <b>io_and_pipe.h</b> and <b>io_and_pipe.c</b>
- <b>Input/Output Redirection (handle_redirection):</b> This function processes arguments to check for input (<) and output (> or >>) redirection. It opens the specified files(with respect to the current working directory) and uses dup2 to redirect the standard input or output to the respective file descriptors. After redirection, the arguments list is modified to remove the redirection symbols.
- <b>Piped Command Execution (execute_piped_commands):</b> This function manages the execution of multiple commands connected by pipes. It first validates the pipe usage (e.g., no pipe at the start or end of the command, no empty command between pipes). It splits the input command by the pipe symbol and executes each segment in a child process, with pipes connecting the output of one command to the input of the next. The function ensures that input/output redirection and background processes are handled appropriately. It waits for all child processes to complete execution before returning control to the parent.
- Piping is not working for custom commands.

## Specification 13 : activities
- The corresponding files are <b>activities.h</b> and <b>activities.c</b>
- It prints a list of all the processes currently running that were spawned by your shell in lexicographic order of pids. This list contains the following information about all processes :
    - Command Name
    - pid
    - state : running or stopped
- Format of output is 
[pid] : [command name] - [State]
- addProcess: Adds a background process to the bgProcessList. It records the process ID (pid), the command string, and the process state (e.g., "Running" or "Stopped").
- updateProcessState: Updates the state of a background process based on its pid. It iterates through the list to find the process and modifies its state accordingly (e.g., from "Running" to "Stopped").
- compareProcesses: A comparator function used by qsort to sort the bgProcessList lexicographically by the pid.
- displayActivities: Displays the background processes in lexicographical order of their pids. For each process, it prints the process ID, command, and current state.

## Specification 14 : Signals
- The corresponding files are <b>ping.h</b> and <b>ping.c</b>
- Implements signal handling and a custom ping command in a shell, enabling users to send signals to both foreground and background processes and handle key combinations like Ctrl+C, Ctrl+D, and Ctrl+Z.
- The ping command allows users to send a signal to a process by providing its PID and the signal number.
- The signal number provided is taken modulo 32 to limit the range of signals. If the signal is successfully sent, a confirmation message is printed.
- If the process does not exist, the command prints "No such process found." If the signal sending fails, an appropriate error message is displayed.
- Ctrl+C (SIGINT): This sends a SIGINT signal to the currently running foreground process, terminating it. If no foreground process exists, the key press is ignored.
- Ctrl+D: Used to log out of the shell by terminating all running processes. It first kills any foreground process (if present), then sends SIGTERM to all background processes, ensuring that the shell exits cleanly. (This is not working as expected).
- Ctrl+Z is not implemented and hence when pressed exits from the shell.
- The function remove_process removes a process from the background process list when it is terminated (e.g., via SIGKILL).

## Specification 16 : Neonate
- Command : neonate -n [time_arg]
- The corresponding files are <b>neonate.h</b> and <b>neonate.c</b>
- The neonate command is designed to print the Process ID (PID) of the most recently created process on the system at regular intervals, as specified by the user. The command continues printing every [time_arg] seconds until the user presses the 'x' key. The implementation avoids using external system programs and instead reads the /proc/loadavg file to retrieve the most recent PID.
- The function get_most_recent_pid() reads from the /proc/loadavg file to get the most recent PID. The /proc/loadavg file contains system load information, with the most recent process PID available as the last field. The function parses this file to extract and return the PID.
- The functions disable_buffering() and restore_buffering() are used to manipulate terminal settings, disabling and restoring canonical mode and echoing, respectively. This allows the program to detect individual keypresses.
- The function is_x_pressed() sets the terminal to non-blocking mode to check if the 'x' key is pressed without waiting for user input, enabling real-time detection.
- After disabling buffering, the program enters an infinite loop, fetching and printing the most recent PID every [time_arg] seconds using sleep(). If the 'x' key is pressed, the loop terminates, restoring the terminal settings and ending the command execution.
- If the /proc/loadavg file cannot be read or an invalid time argument is provided, appropriate error messages are displayed.

## Specification 17 : iMan
- The corresponding files are <b>iMan.h</b> and <b>iMan.c</b>
- The iMan command fetches and displays man pages from the internet using sockets. It connects to the website http://man.he.net/ and retrieves the man page for a specified command. The program sends an HTTP GET request to the server and processes the received HTML response to extract and display the content.
- The command follows the format: iMan <command_name>, where <command_name> is the man page you wish to retrieve.
- The program sends a GET request for the specified command to the server. The response is processed by stripping out the HTTP headers and extracting the body.
- HTML tags within the body content are stripped, leaving only the text of the man page, which is then printed to the terminal.
- If the requested man page does not exist, the response still gets printed, showing the "No matches" message from the server.
- The command accepts only the first argument. Any extra arguments provided are ignored.
- There is no special error handling for "man page not found" scenarios.