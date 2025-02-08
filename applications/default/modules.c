/**
 * @file       modules.c
 *
 * @brief      Try to run a mdule
 *
 * @author     Paul Robson
 *
 * @date       08/02/2025
 *
 */

#include "common.h"
#include "default.h"

void FTH_ApplicationRun(char *params);

static void CMDListModules(void);

//
//      List of all the installed modules and the function to call to run them.
//
struct _ModuleList {
    char *moduleName;
    APPFUNCTION startFunction;
} modules[] = {
    { "forth",FTH_ApplicationRun },
    { NULL,NULL }   
};

/**
 * @brief      Attempt to run the named module
 *
 * @param      cmd   command for module
 *
 * @return     true if module ran.
 */
bool CMDRunModules(char *cmd) {
    int i = 0;
    int size = 0;
    if (*cmd == '\0') {                                                             // No command
        CMDListModules();                                                           // List modules
        return true;
    }    
    while (cmd[size] != '\0' && cmd[size] != ' ') size++;                           // Find length of first word
    char *params = cmd+size;                                                        // Work out parameters
    while (*params == ' ') params++;
    while (modules[i].startFunction != NULL) {                                      // Scan module list
        if (strncmp(modules[i].moduleName,cmd,size) == 0) {                         // Matches first word
            (*modules[i].startFunction)(params);                                    // Call handler.
            return true;
        }
        i++;
    }
    return false;
}

/**
 * @brief      List all modules installed.
 *
 * @param      params  parameters (will be "")
 */
static void CMDListModules(void) {
    int i = 0;
    VDUWriteString("Installed modules:\r\n");
    while (modules[i].startFunction != NULL) {
        if (modules[i].moduleName[0] != '\0') {
            VDUWriteString("    %s\r\n",modules[i].moduleName);
        }
        i++;
    }
}