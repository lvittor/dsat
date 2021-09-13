#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <semaphore.h>
#include <string.h>
#include "app.h"
#include "utils.h"

int main(int argc, char * argv[]) {
    int nfiles = 0, correct = 0;
    if (argc > 1)
        correct = sscanf(argv[1], "%d", &nfiles);
    else
        correct = scanf("%d", &nfiles);

    if (correct < 1)
        fexit("Error: Couldn't parse number of files");

    int fdShm = shm_open(SHARED_MEM_NAME, O_RDWR | O_CREAT, 0660);
    if (fdShm == -1)
        fexit("Error: Couldn't create shared mem");

    int shmSize = nfiles * MAX_SLAVE_OUTPUT;

    if (ftruncate(fdShm, shmSize) == -1)
       fexit("Error: Couldn't truncate shared mem");
    
    char * pshm = mmap(NULL, shmSize, PROT_WRITE, MAP_SHARED, fdShm, 0);
    if (pshm == MAP_FAILED)
       fexit("Error: Couldn't map shared mem");
    
    if (close(fdShm))
        fexit("Error: Couldn't close shared mem file descriptor");

    sem_t * semaphore = sem_open(SEM_COUNT_NAME, O_CREAT, S_IRUSR | S_IWUSR, 0);
    if (semaphore == SEM_FAILED)
        fexit("Error: Couldn't create semaphore");

    char * rpshm = pshm;
    for (int i = 0; i < nfiles; i++) {
        int c = -1;
        sem_getvalue(semaphore, &c);
        sem_wait(semaphore);
        rpshm += printf("%s\n", rpshm);
        fflush(stdout);
    }

    if (sem_close(semaphore))
        fexit("Error: Couldn't close semaphore");

    if (munmap(pshm, shmSize))
        fexit("Error: Couldn't unmap shared mem");
}