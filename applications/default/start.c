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

void FTH_ApplicationRun(void);

/**
 * @brief      Run the main program
 */
void ApplicationRun(void) {
    // FTH_ApplicationRun();
    char inputLine[64];
    while (SYSAppRunning()) {
        VDUWrite(42);VDUWrite(32);
        int linePos = 0;
        int c;
        while (c = KBDGetKey(),c != 13 && SYSAppRunning()) {
            if (c >= ' ' && c < 0x7F && linePos < sizeof(inputLine)-1) {
                c = tolower(c);
                inputLine[linePos++] = c;
                inputLine[linePos] = '\0';
                VDUWrite(c);
            }
            SYSYield();
        }
        VDUWrite(13);VDUWrite(10);
        if (strcmp(inputLine,"forth") == 0) FTH_ApplicationRun();
    }
}
