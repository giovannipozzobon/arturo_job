/**
 * @file       cwd.c
 *
 * @brief      Current working directory handling code.
 *
 * @author     Paul Robson
 *
 * @date       09/02/2025
 */

#include "common.h"

static char currentDirectory[128] = { "/" };                                        // The current directory
static char fileBuffer[sizeof(currentDirectory)+MAX_FILENAME_SIZE];                 // Buffer for result file name

/**
 * @brief      Map simple file name onto CWD if required
 *
 * @param      fileName  file name
 *
 * @return     full path file name.
 */
char *FIOMapFileName(char *fileName) {

    if (fileName[0] == '/') return fileName;                                        // Path already provided.

    strcpy(fileBuffer,currentDirectory);                                            // Build full filename, adding / if required.
    if (currentDirectory[strlen(currentDirectory)-1] != '/') {
        strcat(currentDirectory,"/");
    }
    strcat(fileBuffer,fileName);
    return fileName;
}

/**
 * @brief      Get the current working directory
 *
 * @return     the CWD
 */
char *FIOGetCWD(void) {
    return currentDirectory;
}

/**
 * @brief      Change current working directory
 *
 * @param      d     change to make (abs path, rel path, "", "." or "..")
 *
 * @return     non zero if error.
 */
int FIOChangeCWD(char *d) {
    printf("[%s]\n",d);
    return 0;
}
