#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>
#include <stdio.h>

#include "shmADT.h"

#define SEM_COUNT_NAME  "/sem"

typedef struct shmCDT {
    const char * name;  // name of the shm
    off_t length;       // lenght og the shm
    char * pshm;        // pointer to the shm
    sem_t * semaphore;  // semaphore of the shm
    char * dpshm;       // dynamic pointer to the shm
} shmCDT;

/**
 * @brief returns EXIT_ERROR and free's the shm
 * 
 * @param shm pointer to the shm
 * @return int EXIT_ERROR
 */
static int safeReturn(shmADT shm){
    freeShm(shm);
    return SHM_ERROR;
}

shmADT newShm() {
    return calloc(1, sizeof(shmCDT));
}

int openAndMapShm(shmADT shm, const char * name, int oflag, mode_t mode, off_t length, int mProt, int mFlags) {
    shm->name = name;
    shm->length = length;
    
    int fdShm = shm_open(shm->name, oflag, mode);
    if (fdShm == -1)
        return safeReturn(shm);

    if (ftruncate(fdShm, shm->length) == -1)
        return safeReturn(shm);
    
    shm->pshm = mmap(NULL, shm->length, mProt, mFlags, fdShm, 0);
    if (shm->pshm == MAP_FAILED)
        return safeReturn(shm);
        
    shm->dpshm = shm->pshm;
    
    if (close(fdShm))
        return safeReturn(shm);

    shm->semaphore = sem_open(SEM_COUNT_NAME, O_CREAT, S_IRUSR | S_IWUSR, 0);
    if (shm->semaphore == SEM_FAILED) {
        munmap(shm->pshm, shm->length);
        return safeReturn(shm);
    }

    return SHM_SUCCESS;
}

int writeShm(shmADT shm, const char * s) {
    shm->dpshm += sprintf(shm->dpshm, "%s", s) + 1;
    sem_post(shm->semaphore);
    return SHM_SUCCESS;
}

int readShm(shmADT shm, char * s) {
    sem_wait(shm->semaphore);
    shm->dpshm += sprintf(s, "%s", shm->dpshm) + 1;
    return SHM_SUCCESS;
}

int unmapShm(shmADT shm){
    if (sem_close(shm->semaphore) || munmap(shm->pshm, shm->length))
        return safeReturn(shm);
    return SHM_SUCCESS;
}

int unlinkShm(shmADT shm) {
    if (sem_unlink(SEM_COUNT_NAME) || shm_unlink(shm->name)) 
        return safeReturn(shm);
    return SHM_SUCCESS;
}

void freeShm(shmADT shm) {
    free(shm);
}