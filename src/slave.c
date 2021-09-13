#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "app.h"

#define BUFFER_SIZE 50
#define CMD_SIZE 1024
#define CMD "minisat"
#define FILTER "grep -o -e \"Number of.*[0-9]\\+\" -e \"CPU time.*\" -e \".*SATISFIABLE\""
#define DELIMITER ", "

int main(int argc, char *argv[]) {
    FILE * fp;
    char buffer[BUFFER_SIZE];
    char pathBuffer[BUFFER_SIZE];
    char cmd[CMD_SIZE];
    char output[MAX_SLAVE_OUTPUT];

    while (! fgetsn(pathBuffer, BUFFER_SIZE, stdin)) {
        if (fileIsReg(pathBuffer)) {
            if (getCommand(cmd, CMD_SIZE, pathBuffer, CMD, FILTER)) 
                fexit("Error: couldn't concatenate command to file or filter strings");

            if ((fp = popen(cmd, "r")) == NULL)
                fexit("Error: couldn't run cmd");
            
            snprintf(output, MAX_SLAVE_OUTPUT, "Filename:\t%s", pathBuffer);
            while (! fgetsn(buffer, BUFFER_SIZE, fp)) {
                strncat(output, DELIMITER , MAX_SLAVE_OUTPUT);
                strncat(output, buffer, MAX_SLAVE_OUTPUT);
            }

            snprintf(buffer, BUFFER_SIZE, "%sPID:\t%ld", DELIMITER, (long)getpid());
            strncat(output, buffer, MAX_SLAVE_OUTPUT);
            
            printf("%s\n", output);
            fflush(stdout);

        } else
            perror("Error: given path is not a regular file");
    }

    return EXIT_SUCCESS;
}