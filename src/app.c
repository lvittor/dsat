#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/select.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stddef.h>

#include "app.h"
#include "utils.h"
#include "shmADT.h"
#include "dispatcherADT.h"

int main(int argc, char * argv[]) {
    if (argc < 2)
        fexit("Error: missing command line arguments");
    
    int fileCount = argc - 1;
    int shmSize = fileCount * MAX_SLAVE_OUTPUT;
    
    shmADT shm = newShm();
    if (shm == NULL)
        fexit("Error: Couldn't create shared memory ADT");
        
    if (openAndMapShm(shm, SHARED_MEM_NAME, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR, shmSize, PROT_WRITE, MAP_SHARED) == SHM_ERROR)
        fexit("Error: Couldn't open or map shared memory");

    printf("%d\n", fileCount);
    fflush(stdout);

    dispatcherADT dispatcher = newDispatcher("out/slave", NUMBER_SLAVES_REQUEST, argv + 1, fileCount);
    if (dispatcher == NULL)
        fexit("Error: Couldn't create dispatcher ADT");
    
    if (createSlaves(dispatcher) == DISPATCHER_ERROR)
        fexit("Error: Couldn't initialize slaves");

    if (beginDispatching(dispatcher) == DISPATCHER_ERROR)
        fexit("Error: Couldn't dispatch files");

    sleep(WAIT_SECONDS_VIEW);

    FILE * outputFile = fopen(OUTPUT_FILE_NAME, "w");
    if (outputFile == NULL)
        fexit("Error: Coudln't open output file");

    char buffer[MAX_SLAVE_OUTPUT];
    for (int filesReceived = 0; filesReceived < fileCount; ) {
        if (slaveSelect(dispatcher) == DISPATCHER_ERROR)
            fexit("Error: Select failed");
        while(canReadFromSlave(dispatcher)) {
            int nRead = readFromSlave(dispatcher, buffer, MAX_SLAVE_OUTPUT);
            filesReceived++;
            if (nRead >= 1) {
                writeShm(shm, buffer);
                fprintf(outputFile, "%s\n", buffer);
            }
        }
    }

    if (waitForChildren(dispatcher) == DISPATCHER_ERROR)
        fexit("Error: Couldn't wait for slave processes");
    
    freeDispatcher(dispatcher);

    if (fclose(outputFile))
        fexit("Error: Couldn't close output file");    

    if (unmapShm(shm) == SHM_ERROR || unlinkShm(shm) == SHM_ERROR)
        fexit("Error: Couldn't unmap or unlink shm");

    freeShm(shm);

    return EXIT_SUCCESS;
}


