/**
 * @file dispatcherADT.h
 * @author Mateo Bartellini Huapalla (@mbartellini), Franco Nicolas Estevez (@festevezz), Lucas Agustin Vittor (@lvvittor)
 * @brief This dispatcher Abstract Data Type allows to create slaves and pipes in order to distribute a number of files for processing
 * @version 0.1
 * @date 2021-09-13
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#ifndef __DISPATCHER_ADT_H__
#define __DISPATCHER_ADT_H__

#define DISPATCHER_ERROR -1
#define DISPATCHER_SUCCESS 0

#define INITIAL_FILES_PER_SLAVE 2
#define SLAVE_SEPARATOR "\n"

typedef struct dispatcherCDT * dispatcherADT;

/**
 * @brief Creates an instance of dispatcher
 * 
 * @param slaveProg path to the program the slaves will run
 * @param slaveCount number of slaves
 * @param files array of strings with the file names
 * @param fileCount size of the previous array
 * @return dispatcherADT returns a pointer to the dispatcher
 */
dispatcherADT newDispatcher(const char * slaveProg, int slaveCount, char ** files, int fileCount);


/**
 * @brief Creates pipes and children processes
 * 
 * @param dispatcher a dispatcherADT previously created 
 * @return int returns DISPATCHER_ERROR on error and DISPATCHER_SUCCESS on success
 */
int createSlaves(dispatcherADT dispatcher);

/**
 * @brief Sends initial files to slaves
 * 
 * @param dispatcher a dispatcherADT previously created 
 * @return int returns DISPATCHER_ERROR on error and DISPATCHER_SUCCESS on success
 */
int beginDispatching(dispatcherADT dispatcher);

/**
 * @brief Monitors all pipe file descriptors, waiting until one or more become "ready" for reading
 * 
 * @param dispatcher a dispatcherADT previously created 
 * @return int returns DISPATCHER_ERROR on error and DISPATCHER_SUCCESS on success
 */
int slaveSelect(dispatcherADT dispatcher);

/**
 * @brief Checks if there are slaves with output ready, should always be called before readFromSlave
 * 
 * @param dispatcher a dispatcherADT previously created 
 * @return char true or false
 */
char canReadFromSlave(dispatcherADT dispatcher);

/**
 * @brief Reads at most n characters from an available slave
 * 
 * @param dispatcher a dispatcherADT previously created 
 * @param buffer buffer to be written to
 * @return int returns -1 on error or the number of characters read
 */
int readFromSlave(dispatcherADT dispatcher, char * buffer, int n);

/**
 * @brief Waits for all slaves created to return.
 * 
 * @param dispatcher a dispatcherADT previously created 
 * @return int returns DISPATCH_ERROR on ERROR or DISPATCH_SUCCESS on success
 */
int waitForChildren(dispatcherADT dispatcher);

/**
 * @brief Frees memory, should be called after receiving fileCount responses using slaveSelect and readFromSlave
 * 
 * @param dispatcher a dispatcherADT previously created 
 */
void freeDispatcher(dispatcherADT dispatcher);


#endif