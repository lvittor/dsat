#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdio.h>

#define MAX_SLAVE_OUTPUT 256

/**
 * @brief macros for returning the max and min 
 *        element between a and b
 * 
 */
#define max(a,b) ((a) > (b) ? (a) : (b))
#define min(a,b) ((a) < (b) ? (a) : (b))

/**
 * @brief prints the pid of the current process
 * 
 */
void printpid();

/**
 * @brief checks if the given path is a regular file.
 * 
 * @param path the path of the file
 * @return int 1 if regular, 0 if not. In case of error returns 0 (with errno set).
 */
int fileIsReg(const char * path);

/**
 * @brief prints error msg to stdout and exits the process
 *      
 * @param msg  error msg to be printed
 */
void fexit(const char * msg);

/**
 * @brief does fgets() and removes the new line
 * 
 * @param buff buffer for fgets()
 * @param n number of chr to read
 * @param stream stream for fgets()
 * @return int < 0 if is an error with fgets(), 0 if success
 */
int fgetsn(char * buff, int n, FILE * stream);

/**
 * @brief concatenate the string of a command with a path argument and a grep-style filter for the output.
 *        cmdBuff results in a string like: cmd "path" | filter
 * 
 * @param cmdBuff buffer for the result command
 * @param cmdSize size of the result command buffer
 * @param path path argument to the command
 * @param cmd command to be executed
 * @param filter grep-style filter for the command output
 * @return int < 0 if is an error with the concatenation of the strings, 0 if success
 */
int getCommand(char *cmdBuff, int cmdSize, const char *path, const char *cmd, const char *filter);

#endif