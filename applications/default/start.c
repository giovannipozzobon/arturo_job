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
    FTH_ApplicationRun();
    // char inputLine[64];
    // while (1) {
    //     VDUWrite(42);VDUWrite(32);
    //     int linePos = 0;
    //     int c;
    //     while (c = KBDGetKey(),c != 13) {
    //         if (c >= ' ' && c < 0x7F && linePos < sizeof(inputLine)-1) {
    //             c = tolower(c);
    //             inputLine[linePos++] = c;
    //             VDUWrite(c);
    //         }
    //         if (HASTICK50_FIRED()) {                                                    // Time to do a 50Hz tick (Don't use this for timing !)
    //             TICK50_RESET();                                                         // Reset the tick flag
    //             if (USBUpdate() == 0) return;                                           // Update USB, exit simulator
    //             KBDCheckTimer();                                                        // Check for keyboard repeat
    //         }
    //     }
    //     VDUWrite(13);VDUWrite(10);
    // }
}
