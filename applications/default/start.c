/**
 * @file       start.c
 *
 * @brief      Start the command line interface
 *
 * @author     Paul Robson
 *
 * @date       07/02/2025
 *
 */

#include "common.h"
#include "default.h"

/**
 * @brief      Read a single keystroke
 *
 * @return     ASCII character of key or 0 if simulator ended
 */
static int CMDGetKey(void) {
    while (SYSAppRunning()) {
        int c = KBDGetKey();
        if (c != 0) return c;
        SYSYield();
    }
    return 0;
}

/**
 * @brief      Read a line into a buffer, all converted to lower case, no
 *             leading spaces
 *
 * @param      buffer   Buffer to use
 * @param[in]  maxSize  Maximum line size
 */
static void CMDReadLine(char *buffer,int maxSize) {
    int c,pos = 0;
    while (c = CMDGetKey(), c != 13 && SYSAppRunning()) {
        if (c == 8 || c == 0x7F) {
            if (pos > 0) {
                VDUWrite(0x7F);pos--;
            }
        } else {
            if (c >= ' ' && pos < maxSize && (c != ' ' || pos != 0)) {
                buffer[pos++] = tolower(c);
                VDUWrite(tolower(c));
            }
        }
    }
    VDUWrite(13);VDUWrite(10);
    buffer[pos] = '\0';
}

/**
 * @brief      Run the main program
 */
void ApplicationRun(void) {
    char inputLine[64];
    while (SYSAppRunning()) {
        VDUWrite(42);VDUWrite(32);
        CMDReadLine(inputLine,sizeof(inputLine)-1);
        if (!CMDRunModules(inputLine)) {
            VDUWriteString("Bad command\r\n");
        }
    }
}
