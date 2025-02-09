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
    if (strcmp(cmd,"pwd") == 0) {                                                   // pwd, print current directory.
        CONWriteString("%s\r\n",FIOGetCWD());
        return true;
    }
    if (strcmp(cmd,"cd") == 0 || strcmp(cmd,"cdir") == 0) {                         // CDIR is MOS standard, CD is more common.
        int e = FIOChangeCWD(params);                                               // Try to change directory.
        if (e != 0) CONWriteString("%c%cFailed.\r\n",17,1);                         // Couldn't do so.
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
    int h = FIOOpenDirectory(FIOGetCWD());
    while (e = FIOReadDirectory(h,buffer),e == 0) {
        if (buffer[0] != '.') {
            FIOInfo fInfo;
            char *fullPath = FIOMapFileName(buffer);
            FSYSFileInformation(fullPath,&fInfo);
            CONWriteString("%-24s ",buffer);
            if (fInfo.isDirectory) {
                CONWriteString("<dir>\r\n");
            } else {
                CONWriteString("%d\r\n",fInfo.length);
            }
        }
    }    
    FIOCloseDirectory(h);
}

// *COPY <src> <dest> [cp ?]
// *DELETE <file> [rm ?]
// *REMOVE <file> [no error if absent]
// *KEY <n> <string>
// *MOVE <src> <dest> [mv ?]
// *RENAME <old> <new> 
