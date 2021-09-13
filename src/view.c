#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <semaphore.h>
#include <string.h>
#include "app.h"
#include "utils.h"
#include "shmADT.h"

int main(int argc, char * argv[]) {
    int nfiles = 0, correct = 0;
    char buffer[MAX_SLAVE_OUTPUT];
    if (argc > 1)
        correct = sscanf(argv[1], "%d", &nfiles);
    else
        correct = scanf("%d", &nfiles);

    if (correct < 1)
        fexit("Error: Couldn't parse number of files");

    shmADT shm = newShm();
    if (shm == NULL)
        fexit("Error: Couldn't create shared memory ADT");
        
    if (openAndMapShm(shm, SHARED_MEM_NAME, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR, nfiles * MAX_SLAVE_OUTPUT, PROT_WRITE, MAP_SHARED) == SHM_ERROR)
       fexit("Error: Couldn't map shared mem");

    for (int i = 0; i < nfiles; i++) {
        readShm(shm, buffer);
        printf("%s\n", buffer);
        fflush(stdout);
    }
    
    if (unmapShm(shm) == SHM_ERROR)
        fexit("Error: Couldn't unmap or unlink shm");

    freeShm(shm);

    return 0;
}