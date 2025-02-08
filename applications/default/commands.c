/**
 * @file       commands.c
 *
 * @brief      CLI commands.
 *
 * @author     Paul Robson
 *
 * @date       08/02/2025
 *
 */

#include "common.h"
#include "default.h"

static void _CMDCLIDirectory(void);

/**
 * @brief      Execute a system command line command
 *
 * @param      cmd   command
 *
 * @return     true if command was executed.
 */
bool CMDCLICommand(char *cmd) {
    char *params = cmd;
    while (*params != ' ' && *params != '\0') params++;                             // Find the end of the command
    if (*params == ' ') {                                                           // Found parameters
        *params++ = '\0';                                                           // End command.
        while (*params == ' ') params++;                                            // Skip spaces.
    }    
    if (strcmp(cmd,".") == 0 || strcmp(cmd,"cat") == 0) {                           // . or cat, show directory.
        _CMDCLIDirectory();
        return true;
    }

    return false;
}

/**
 * @brief      Display the current directory contents.
 */
static void _CMDCLIDirectory(void) {
    int e = 0;
    char buffer[MAX_FILENAME_SIZE];
    int h = FIOOpenDirectory("/");
    while (e = FIOReadDirectory(h,buffer),e == 0) {
        FIOInfo fInfo;
        FSYSFileInformation(buffer,&fInfo);
        CONWriteString("%c %-30s %-6d\r\n",fInfo.isDirectory ? 'D':' ',buffer,fInfo.length);
    }
    FIOCloseDirectory(h);
}