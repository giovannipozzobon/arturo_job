/**
 * @file       main.c
 *
 * @brief      Simulator main program
 *
 * @author     Paul Robson
 *
 * @date       07/01/2025
 *
 */


#include "artsim.h"

bool isAppRunning = true;

#define FRAME_RATE  (50)

static int nextUpdateTime = 0;

/**
 * @brief      Is the app still running (for simulator)
 *
 * @return     true if the app is still running.
 */
bool SYSAppRunning(void) {
    return isAppRunning;
}


/**
 * @brief      Yield, body executed at 50Hz
 *
 * @return     true if 50Hz tick occurred.
 */
bool SYSYield(void) {
    if (TMRReadTimeMS() >= nextUpdateTime) {                                    // So do this to limit the repaint rate to 50Hz.
        nextUpdateTime = TMRReadTimeMS()+100/FRAME_RATE;
        if (SYSPollUpdate() == 0) isAppRunning = false;
        KBDCheckTimer();                                                        // Check for keyboard repeat
        return true;
    }
    return false;
}

/**
 * @brief      Main program.
 *
 * @param[in]  argc  The count of arguments
 * @param      argv  The arguments array
 *
 * @return     { description_of_the_return_value }
 */
int main(int argc,char *argv[]) {
    VDUWrite(22);VDUWrite(DVI_MODE_640_240_8);                                      // Initialise display
    HDRDisplay();                                                                   // Display header    
    CONWriteString("Simulator booting\r\n\r\n");
    KBDReceiveEvent(0,0xFF,0);                                                      // Initialise keyboard manager
    FIOInitialise();                                                                // Initialise file system
    SYSOpen(false);                                                                 // Start SDL and Mouse/Controller/Sound that use it
    ApplicationRun();                                                               // Run the program
    SYSClose();                                                                     // Close down
    return(0);
}
