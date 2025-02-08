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

void FTH_ApplicationRun(void);

static void CMDListModules(void);

//
//      List of all the installed modules and the function to call to run them.
//
struct _ModuleList {
    char *moduleName;
    APPFUNCTION startFunction;
} modules[] = {
    { "forth",FTH_ApplicationRun },
    { "",CMDListModules },
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
    while (modules[i].startFunction != NULL) {
        if (strcmp(modules[i].moduleName,cmd) == 0) {
            (*modules[i].startFunction)();
            return true;
        }
        i++;
    }
    return false;
}

/**
 * @brief      List all modules installed.
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