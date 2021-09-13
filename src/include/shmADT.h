/**
 * @file shmADT.h
 * @author Mateo Bartellini Huapalla (@mbartellini), Franco Nicolas Estevez (@festevezz), Lucas Agustin Vittor (@lvvittor)
 * @brief This shared memory Abstract Data Type allows for a single writer and a single reader to communicate strings between each other.
 * @version 0.1
 * @date 2021-09-13
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#ifndef __SHM_ADT_H__
#define __SHM_ADT_H__

#include <sys/types.h>

#define SHM_ERROR  -1
#define SHM_SUCCESS 0

typedef struct shmCDT * shmADT;

/**
 * @brief creates a new shared memory object
 * 
 * @return shmADT a pointer to the shared memory
 */
shmADT newShm();

/**
 * @brief Opens the shared memory and map it to the current process
 * 
 * @param shm a shmADT previously created
 * @param name name to be given to the shared memory
 * @param oflag bit mask created by ORing exactly one of O_RDONLY or O_RDWR with O_CREAT, O_EXCL, O_TRUNC
 * @param mode permissions given to the file if O_CREAT is used and the file is indeed created
 * @param length length of the shared memory
 * @param mProt permissions given to the mapping: PROT_EXEC, PROT_READ, PROT_WRITE, PROT_NONE
 * @param mFlags determines how updates to the mapping are visible to other processes
 * @return int returns SHM_ERROR on error and SHM_SUCCESS on success
 */
int openAndMapShm(shmADT shm, const char * name, int oflag, mode_t mode, off_t length, int mProt, int mFlags);

/** 
 * @brief Writes into the shared memory, consecutive calls will be appended
 * 
 * @param shm a shmADT previously created
 * @param s string to be written into the shared memory
*/
int writeShm(shmADT shm, const char * s);

/** 
 * @brief Blocking function that reads from the shared memory
 *        After consecutive calls, will read from last read position
 * 
 * @param shm a shmADT previously created
 * @param s string to be read from the shared memory
*/
int readShm(shmADT shm, char * s);

/**
 * @brief Unmaps the shared memory
 * 
 * @param shm a shmADT previously created
 * @return int returns SHM_ERROR on error and SHM_SUCCESS on success
 */
int unmapShm(shmADT shm);

/**
 * @brief Unlinks the shared memory
 * 
 * @param shm a shmADT previously created
 * @return int returns SHM_ERROR on error and SHM_SUCCESS on success
 */
int unlinkShm(shmADT shm);


/**
 * @brief frees the shared memory
 * 
 * @param shm a shmADT previously created
 */
void freeShm(shmADT shm);

#endif