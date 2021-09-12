#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/select.h>

#include "app.h"
#include "utils.h"

typedef struct pipes_t{
    int out[SLAVES][2];
    int in[SLAVES][2];
} pipes_t;

enum PIPE { READ = 0, WRITE };

int main(int argc, char * argv[]) {
    if (argc < 2)
        fexit("Error: missing command line arguments");

    printpid();
    pid_t pids[SLAVES];
    pipes_t appPipes;
    // fd_set readfds, writefds, exceptfds;

    for (int i = 0; i < SLAVES; i++) {
        if (pipe(appPipes.out[i]) || pipe(appPipes.in[i])) 
            fexit("Error: Couldn't create pipe");
        
        printf("SLAVE %d\n", i);
        printf("{or: %d, ow: %d, ir: %d, iw: %d}\n", appPipes.out[i][READ], appPipes.out[i][WRITE], appPipes.in[i][READ], appPipes.in[i][WRITE]);
        printf("--------------------------------------\n");

        if ((pids[i] = fork()) < 0)
            fexit("Error: Couldn't create child process");
            
        else if (pids[i] == 0) { // slave
            close(appPipes.in[i][READ]);
            close(appPipes.out[i][WRITE]);

            dup2(appPipes.out[i][READ], STDIN_FILENO);
            dup2(appPipes.in[i][WRITE], STDOUT_FILENO);
            // dup2(appPipes.in[i][WRITE], STDERR_FILENO);

            execl("out/slave", "slave", (char *)NULL);
        } else { // app
            // close(appPipes.out[i][READ]);
            // close(appPipes.in[i][WRITE]);
        }
    }

    for (int i = 0; i < SLAVES; i++) {
        close(appPipes.out[i][READ]);
        close(appPipes.in[i][WRITE]);
        
        write(appPipes.out[i][WRITE], argv[i+1], strlen(argv[i+1]));
    }

    sleep(5);
    printf("Desperte!\n");
    // leer usando el select
    // for (int i = 0; i < SLAVES; i++) {
    int ready, nfds = 0;
    ssize_t nbytes;
    fd_set readfds;
    char buffer[50];

    //int currfd = appPipes.in[i][READ];

    FD_ZERO(&readfds);
    for (int j = 0; j < SLAVES; j++) {
        FD_SET(appPipes.in[j][READ], &readfds);
        nfds = max(nfds, appPipes.in[j][READ]); 
    }

    printf("Por entrar al select...\n");
    ready = select(nfds + 1, &readfds, NULL, NULL, NULL);
    printf("Saliendo del select...\n");
    if (ready == -1)
        fexit("Error: select()");

    for (int j = 0; j < SLAVES; j++) {
        int fd = appPipes.in[j][READ];
        
        if (FD_ISSET(fd, &readfds)) {
            nbytes = read(fd, buffer, 50);

            if (nbytes >= 1){
                printf("{%s}\n", buffer);
                fflush(stdout);
            }
        }
    }
    // }


    return EXIT_SUCCESS;
}