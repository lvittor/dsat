#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/select.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "app.h"
#include "utils.h"

typedef struct pipes_t{
    int out[MAX_SLAVE_COUNT][2];
    int in[MAX_SLAVE_COUNT][2];
} pipes_t;

enum PIPE { READ = 0, WRITE };

int main(int argc, char * argv[]) {
    if (argc < 2)
        fexit("Error: missing command line arguments");
    
    int nfiles = argc - 1;
    printf("%d\n", nfiles);
    fflush(stdout);
    int filesSent = 0;
    int nslaves = min(nfiles, MAX_SLAVE_COUNT);
    int shmSize = nfiles * MAX_SLAVE_OUTPUT;

    FILE * outputFile = fopen(OUTPUT_FILE_NAME, "w");
    if (outputFile == NULL)
        fexit("Error: Coudln't open output file");
    
    int fdShm = shm_open(SHARED_MEM_NAME, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (fdShm == -1)
        fexit("Error: Couldn't create shared mem");
    
    if (ftruncate(fdShm, shmSize) == -1)
       fexit("Error: Couldn't truncate shared mem");
    
    char * pshm = mmap(NULL, shmSize, PROT_WRITE, MAP_SHARED, fdShm, 0);
    char * wpshm = pshm;

    if (pshm == MAP_FAILED)
       fexit("Error: Couldn't map shared mem");
    
    if (close(fdShm))
        fexit("Error: Couldn't close shared mem file descriptor");

    sem_t * semaphore = sem_open(SEM_COUNT_NAME, O_CREAT, S_IRUSR | S_IWUSR, 0);
    if (semaphore == SEM_FAILED)
        fexit("Error: Couldn't create semaphore");

    sleep(2);

    pid_t pids[MAX_SLAVE_COUNT];
    int workingFiles[MAX_SLAVE_COUNT] = {0};
    pipes_t appPipes;

    for (int i = 0; i < nslaves; i++) {
        if (pipe(appPipes.out[i]) || pipe(appPipes.in[i])) 
            fexit("Error: Couldn't create pipe");

        if ((pids[i] = fork()) < 0)
            fexit("Error: Couldn't create child process");
            
        else if (pids[i] == 0) { // slave
            close(appPipes.in[i][READ]);
            close(appPipes.out[i][WRITE]);

            dup2(appPipes.out[i][READ], STDIN_FILENO);
            dup2(appPipes.in[i][WRITE], STDOUT_FILENO);

            execl("out/slave", "slave", (char *)NULL);
            fexit("Error: Couldn't run slave");
        } else { // app
            close(appPipes.out[i][READ]);
            close(appPipes.in[i][WRITE]);
        }
    }

    if (nfiles >= MAX_SLAVE_COUNT * INITIAL_FILES_PER_SLAVE) {
        for (int i = 0; i < MAX_SLAVE_COUNT; i++) {
            for (int j = 0; j < INITIAL_FILES_PER_SLAVE; j++) {
                write(appPipes.out[i][WRITE], argv[filesSent + 1], strlen(argv[filesSent + 1]) + 1);
                write(appPipes.out[i][WRITE], "\n", 1);
                workingFiles[i]++;
                filesSent++;
            }
        }
    } else {
        for (int i = 0; i < nslaves; i++) {
            write(appPipes.out[i][WRITE], argv[filesSent + 1], strlen(argv[filesSent + 1]) + 1);
            write(appPipes.out[i][WRITE], "\n", 1);
            workingFiles[i]++;
            filesSent++;
        }
    }

    for (int filesReceived = 0; filesReceived < nfiles;) {
        int ready, nfds = 0;
        ssize_t nbytes;
        fd_set readfds;
        char buffer[MAX_SLAVE_OUTPUT];

        FD_ZERO(&readfds);
        for (int j = 0; j < nslaves; j++) {
            if (workingFiles[j] > 0) {
                FD_SET(appPipes.in[j][READ], &readfds);
                nfds = max(nfds, appPipes.in[j][READ]);
            } 
        }

        ready = select(nfds + 1, &readfds, NULL, NULL, NULL);
        
        if (ready == -1)
            fexit("Error: Couldn't perform select");

        for (int j = 0; j < nslaves; j++) {
            int fd = appPipes.in[j][READ];
            if (FD_ISSET(fd, &readfds)) {
                FD_CLR(fd, &readfds);
                filesReceived++;

                nbytes = read(fd, buffer, MAX_SLAVE_OUTPUT);
                buffer[nbytes] = '\0';

                if (nbytes >= 1) {
                    wpshm += sprintf(wpshm, "%s", buffer);
                    wpshm[0] = '\0';
                    wpshm++;
                    fprintf(outputFile, "%s\n", buffer);
                    sem_post(semaphore);
                }
                workingFiles[j]--;
                if (filesSent >= nfiles) {
                    close(appPipes.out[j][WRITE]);
                } else if (workingFiles[j] == 0){
                    write(appPipes.out[j][WRITE], argv[filesSent + 1], strlen(argv[filesSent + 1]) + 1);
                    write(appPipes.out[j][WRITE], "\n", 1);
                    workingFiles[j]++;
                    filesSent++;
                }
            }
        }
    }

    for (int i = 0; i < nslaves; i++)
        if(wait(NULL) < 0)
            fexit("Error: Couldn't wait for child");
        
    if (sem_close(semaphore))
        fexit("Error: Couldn't close semaphore");
    
    if (sem_unlink(SEM_COUNT_NAME))
        fexit("Error: Couldn't unlink semaphore");

    if (munmap(pshm, shmSize))
        fexit("Error: Couldn't unmap shared mem");
    
    if (shm_unlink(SHARED_MEM_NAME))
        fexit("Error: Couldn't unlink shared mem");

    if (fclose(outputFile))
        fexit("Error: Couldn't close output file");

    return EXIT_SUCCESS;
}


