#include <stddef.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>

#include "dispatcherADT.h"
#include "utils.h"

#define FD_PER_PIPE 2

typedef struct dispatcherCDT {
    const char * slaveProg;
    int slaveCount;
    char ** files;
    int fileCount;
    int * out;
    int * in;
    int filesSent;
    int * workingFiles;
    int toRead;
    fd_set readfds;
} dispatcherCDT;

enum PIPE {
    READ = 0,
    WRITE
};

dispatcherADT newDispatcher(const char * slaveProg, int slaveCount, char ** files, int fileCount) {
    if (slaveCount < 1 || fileCount < 1)
        return NULL;
    
    dispatcherADT dispatcher = calloc(1, sizeof(dispatcherCDT));
    if (dispatcher == NULL)
        return NULL;
    
    dispatcher->slaveCount = min(slaveCount, fileCount);
    dispatcher->files = files;
    dispatcher->fileCount = fileCount;
    dispatcher->slaveProg = slaveProg;
    dispatcher->filesSent = 0;

    dispatcher->workingFiles = calloc(dispatcher->slaveCount, sizeof(int)); 
    if (dispatcher->workingFiles == NULL){
        freeDispatcher(dispatcher);
        return NULL;
    }
    
    dispatcher->out = (int *)malloc(slaveCount * FD_PER_PIPE * sizeof(int));
    if (dispatcher->out == NULL) {
        freeDispatcher(dispatcher);
        return NULL;
    }

    dispatcher->in = (int *)malloc(slaveCount * FD_PER_PIPE * sizeof(int));
    if (dispatcher->in == NULL){
        freeDispatcher(dispatcher);
        return NULL;
    }

    return dispatcher;
}

int createSlaves(dispatcherADT dispatcher) {
    for (int i = 0; i < dispatcher->slaveCount; i++) {
        if (pipe(dispatcher->out + FD_PER_PIPE * i) || pipe(dispatcher->in + FD_PER_PIPE * i)) 
            return DISPATCHER_ERROR;

        int pid;
        if ((pid = fork()) < 0)
            return DISPATCHER_ERROR;

        else if (pid == 0) {
            close(dispatcher->in[FD_PER_PIPE * i + READ]);
            close(dispatcher->out[FD_PER_PIPE * i + WRITE]);

            dup2(dispatcher->out[FD_PER_PIPE * i + READ], STDIN_FILENO);
            dup2(dispatcher->in[FD_PER_PIPE * i + WRITE], STDOUT_FILENO);
            execl("out/slave", "slave", (char *)NULL);

            return DISPATCHER_ERROR;

        } else {
            close(dispatcher->out[FD_PER_PIPE * i + READ]);
            close(dispatcher->in[FD_PER_PIPE * i + WRITE]);
        }
    }
    return DISPATCHER_SUCCESS;
}

/**
 * @brief sends a filepath to the slave with the given slaveID
 * 
 * @param dispatcher a pointer to a dispatcher object 
 * @param slaveID the ID of the slave
 * @return int returns DISPATCHER_ERROR on error and DISPATCHER_SUCCESS on success
 */
static int dispatchFile(dispatcherADT dispatcher, int slaveID) {
    if (dispatcher->filesSent >= dispatcher->fileCount || dispatcher->files[dispatcher->filesSent] == NULL)
        return DISPATCHER_ERROR;
        
    const char * file = dispatcher->files[dispatcher->filesSent];
    if (write(dispatcher->out[FD_PER_PIPE * slaveID + WRITE], file, strlen(file) + 1) < 0)
        return DISPATCHER_ERROR; 
        
    if (write(dispatcher->out[FD_PER_PIPE * slaveID + WRITE], SLAVE_SEPARATOR, 1) < 0)
        return DISPATCHER_ERROR;
        
    dispatcher->workingFiles[slaveID]++;
    dispatcher->filesSent++;
    return DISPATCHER_SUCCESS;
}

int beginDispatching(dispatcherADT dispatcher) { 
    if (dispatcher->fileCount >= dispatcher->slaveCount * INITIAL_FILES_PER_SLAVE) {
        for (int i = 0; i < dispatcher->slaveCount; i++) {
            for (int j = 0; j < INITIAL_FILES_PER_SLAVE; j++) {
                if (dispatchFile(dispatcher, i) == DISPATCHER_ERROR)
                    return DISPATCHER_ERROR;
            }
        }
    } else {
        for (int i = 0; i < dispatcher->slaveCount; i++) {
            if (dispatchFile(dispatcher, i) == DISPATCHER_ERROR)
                return DISPATCHER_ERROR;
        }
    }
    return DISPATCHER_SUCCESS;
}

int slaveSelect(dispatcherADT dispatcher) { 
    int nfds = 0;
    FD_ZERO(&(dispatcher->readfds));
    for (int i = 0; i < dispatcher->slaveCount; i++) {
        if (dispatcher->workingFiles[i] > 0) {
            FD_SET(dispatcher->in[FD_PER_PIPE * i + READ], &(dispatcher->readfds));
            nfds = max(nfds, dispatcher->in[FD_PER_PIPE * i + READ]);
        }
    }

    if ((dispatcher->toRead = select(nfds + 1, &(dispatcher->readfds), NULL, NULL, NULL)) < 0)
        return DISPATCHER_ERROR;
    return DISPATCHER_SUCCESS;
}

char canReadFromSlave(dispatcherADT dispatcher) { 
    return dispatcher->toRead > 0;
}

int readFromSlave(dispatcherADT dispatcher, char * buffer, int n) {
    if (! canReadFromSlave(dispatcher) || n < 0)
        return -1;

    ssize_t nbytes = -1;
    for (int i = 0; i < dispatcher->slaveCount; i++) {
        int fd = dispatcher->in[i * FD_PER_PIPE + READ];
        if (FD_ISSET(fd, &(dispatcher->readfds))) {
            FD_CLR(fd, &(dispatcher->readfds));
            dispatcher->toRead--;
            for (nbytes = 0; nbytes < n; nbytes++) {
                if (read(fd, buffer + nbytes, 1) < 1)
                    return -1;
                if (buffer[nbytes] == '\n') {
                    buffer[nbytes] = '\0';
                    break;
                }
            }
            if (nbytes >= 0) {
                buffer[nbytes] = '\0';
                dispatcher->workingFiles[i]--;
                if (dispatcher->filesSent >= dispatcher->fileCount) {
                    close(dispatcher->out[FD_PER_PIPE * i + WRITE]);
                } else if (dispatcher->workingFiles[i] == 0) {
                    if (dispatchFile(dispatcher, i) == DISPATCHER_ERROR)
                        return -1;
                }
                break;
            } else
                return -1;  
        }
    }
    return nbytes;
}


int waitForChildren(dispatcherADT dispatcher) {
    for (int i = 0; i < dispatcher->slaveCount; i++)
        if (wait(NULL) < 0)
            return DISPATCHER_ERROR;
    return DISPATCHER_SUCCESS;
}


void freeDispatcher(dispatcherADT dispatcher) {
    if (dispatcher != NULL) {
        free(dispatcher->in);
        free(dispatcher->out);
        free(dispatcher->workingFiles);
    }
    free(dispatcher);
}
