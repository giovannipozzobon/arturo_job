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
    return fileBuffer;
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
    if (strcmp(d,".") == 0 || strcmp(d,"") == 0) return 0;                          // . or empty string, no change.
    if (d[0] == '/') {                                                              // has a /, it's an absolute path
        strcpy(currentDirectory,d);                                                 // make that the current directory.
        return 0;
    }
    if (strcmp(d,"..") == 0) {                                                      // Up one level.
        char *lastSlash = currentDirectory;                                         // Find the last slash.
        char *p = currentDirectory;
        while (*p != '\0') {
            if (*p == '/') lastSlash = p;
            p++;
        }
        *lastSlash = '\0';                                                          // Truncate at last slash.
        if (*currentDirectory == '\0') strcpy(currentDirectory,"/");                // Reached the root ?
        return 0;
    }

    char *newDir = FIOMapFileName(d);                                               // Get the new thing switched to.
    FIOInfo info;
    int e = FIOFileInformation(newDir,&info);                                       // Try to get information about it.
    if (e != 0) return e;                                                           // Not found.
    if (!info.isDirectory) return FIO_ERR_NOTDIR;                                   // Wrong type
    strcpy(currentDirectory,newDir);                                                // Update cwd
    return 0;
}
