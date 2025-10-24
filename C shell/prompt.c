#include "headers.h"

void printPrompt(const char *cwd,char* home_directory, char* ext_prompt) 
{
    char* username=getlogin();
    char hostname[MAX_LENGTH]; 
    gethostname(hostname,sizeof(hostname)); 

    if(strncmp(cwd,home_directory,strlen(home_directory))==0){
        if(strcmp(cwd,home_directory)==0){
            printf("\033[0;35m<%s@%s:~%s>\033[0m ",username,hostname, ext_prompt);
        } 
        else{
            printf("\033[0;35m<%s@%s:~%s%s>\033[0m ",username,hostname,cwd+strlen(home_directory), ext_prompt);
        }
    } 
    else{
        printf("\033[0;35m<%s@%s:%s%s>\033[0m ",username,hostname,cwd, ext_prompt);
    }
}
