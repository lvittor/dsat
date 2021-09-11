#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"

#define BUFFER_SIZE 50
#define CMD_SIZE 1024
#define CMD "minisat"
#define FILTER "grep -o -e \"Number of.*[0-9]\\+\" -e \"CPU time.*\" -e \".*SATISFIABLE\""

int main(int argc, char *argv[]) {
    FILE * fp;
    char buffer[BUFFER_SIZE];
    char pathBuffer[BUFFER_SIZE];
    char cmd[CMD_SIZE];

    while (!fgetsn(pathBuffer, BUFFER_SIZE, stdin)) {
        if (fileIsReg(pathBuffer)) {
            if (getCommand(cmd, CMD_SIZE, pathBuffer, CMD, FILTER)) 
                fexit("Error: couldn't concatenate command to file or filter strings");

            if ((fp = popen(cmd, "r")) == NULL)
                fexit("Error: couldn't run minisat");
            
            while (fgets(buffer, BUFFER_SIZE, fp) != NULL)
                printf("%s", buffer);

        } else
            perror("Error: given path is not a regular file"); 
    }

    return EXIT_SUCCESS;
}