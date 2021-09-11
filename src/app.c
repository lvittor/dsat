#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <unistd.h>

#include "app.h"
#include "utils.h"

int main(int argc, char * argv[]) {
    if (argc < 2)
        fexit("Error: missing command line arguments");

    printpid();
    pid_t pids[SLAVES];
    int pipes[SLAVES][2];

    for (int i = 0; i < SLAVES; i++) {
        if (pipe(pipes[i])) 
            fexit("Error: Couldn't create pipe");

        if ((pids[i] = fork()) < 0)
            fexit("Error: Couldn't create child process");
            
        else if (pids[i] == 0) { // slave
            close(pipes[i][1]); 
            dup2(pipes[i][0], STDIN_FILENO);
            execl("out/slave", "slave", (char *)NULL);
        } else { // app
            close(pipes[i][0]);
        }
    }

    for (int i = 0; i < SLAVES; i++)
        write(pipes[i][1], argv[i+1], strlen(argv[i+1]));
    
    sleep(5);

    return EXIT_SUCCESS;
}