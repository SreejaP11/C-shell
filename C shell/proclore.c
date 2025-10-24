#include "headers.h"

void printProcInfo(pid_t pid) 
{
    char status;
    char *status_desc;

    char status_path[BUFFER_SIZE];
    snprintf(status_path,sizeof(status_path),"/proc/%d/status",pid);
    
    FILE *status_file=fopen(status_path,"r");
    if(status_file==NULL){
        perror("\033[0;31mfopen\033[0m");
        return;
    }
    
    char line[BUFFER_SIZE];
    while(fgets(line,sizeof(line),status_file)){
        if(strncmp(line,"State:",6)==0){
            sscanf(line+7," %c",&status);
            break;
        }
    }
    fclose(status_file);
    
    switch(status){
        case 'R': status_desc="Running"; break;
        case 'S': status_desc="Sleeping in an interruptible wait"; break;
        case 'Z': status_desc="Zombie"; break;
        default: status_desc="Unknown"; break;
    }

    char stat_path[BUFFER_SIZE];
    snprintf(stat_path,sizeof(stat_path),"/proc/%d/stat",pid);
    
    FILE *stat_file=fopen(stat_path,"r");
    if(stat_file==NULL) {
        perror("\033[0;31mfopen\033[0m");
        return;
    }
    
    int process_group;
    fscanf(stat_file, "%*d %*s %*c %d",&process_group);
    fclose(stat_file);

    char statm_path[BUFFER_SIZE];
    snprintf(statm_path, sizeof(statm_path),"/proc/%d/statm",pid);
    
    FILE *statm_file=fopen(statm_path, "r");
    if(statm_file==NULL) {
        perror("\033[0;31mfopen\033[0m");
        return;
    }
    
    unsigned long vm_size;
    fscanf(statm_file,"%lu",&vm_size);
    fclose(statm_file);
    
    char exe_path[MAX_LENGTH];
    snprintf(exe_path,sizeof(exe_path),"/proc/%d/exe",pid);
    
    char real_path[MAX_LENGTH];
    ssize_t path_len=readlink(exe_path, real_path,sizeof(real_path)-1);
    if(path_len!=-1){
        real_path[path_len]='\0';
    } 
    else{
        perror("\033[0;31mreadlink\033[0m");
        return;
    }

    printf("pid : %d\n", pid);
    printf("process status : %c%c\n", status, (pid == getpid() ? '+' : ' '));
    printf("Process Group : %d\n", process_group);
    printf("Virtual memory : %lu\n", vm_size);
    printf("executable path : %s\n", real_path);
}
