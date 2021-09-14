#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define ERROR -1
#define SUCCESS 0


void printpid(){
    printf("%ld\n", (long)getpid()); 
}

void fexit(const char * msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

int fileIsReg(const char * path){
    struct stat st;
    return stat(path, &st) >= 0 && S_ISREG(st.st_mode);
}

int fgetsn(char * buff, int n, FILE * stream) {
    if (fgets(buff, n, stream) == NULL)
        return ERROR;
    strtok(buff, "\n");
    return SUCCESS;
}

int getCommand(char *cmdBuff, int cmdSize, const char *path, const char *cmd, const char *filter) {
    int i = snprintf(cmdBuff, cmdSize, "%s \"%s\" | %s", cmd, path, filter);
    return (i < 0 || i >= cmdSize) ? ERROR : SUCCESS;
}

