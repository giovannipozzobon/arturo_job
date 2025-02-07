/**
 * @file       test_app.c
 *
 * @brief      Test various kernel functions
 *
 * @author     Paul Robson
 *
 * @date       07/01/2025
 *
 */

#include "common.h"
#include "testapp.h"

int mode = 3;


/**
 * @brief      Set up for text window test
 */
static void _GraphicsTest1(void) {

    VDUWrite(17);VDUWrite(129);VDUWrite(12);                                        // Clear background to red
    VDUWrite(31);VDUWrite(10);VDUWrite(1);


    VDUWrite(23);VDUWrite(0xE0);VDUWrite(0xFF);                                     // Define $E0
    for (int i = 0;i < 6;i++) VDUWrite(0x81);
    VDUWrite(0xFF);

    VDUWriteString("Kernel Demo Application\r\nNext line %c%c\r\n",0xE0,0x2A);                                          
    VDUWrite(17);VDUWrite(132);                                                     // Background blue
    VDUWrite(17);VDUWrite(2);
    VDUWrite(28);VDUWrite(4);VDUWrite(4);VDUWrite(35);VDUWrite(22);                 // Create a working window
    VDUWrite(12);                                                                   // Clear it
}

/**
 * @brief      Do the graphics test.
 */

#define VDUW(x)     { VDUWrite((x) & 0xFF);VDUWrite((x) >> 8); }

static void _Rectangle(int col,int x,int y,int w,int h) {
    VDUSetGraphicsColour(0,col);
    VDUPlotCommand(4,x,y);
    VDUPlotCommand(4,x,y+h);
    VDUPlotCommand(85,x+w,y);
    VDUPlotCommand(85,x+w,y+h);
}

static void _GraphicsTest(void) {
    for (int i = 0;i < 64;i++) {
        _Rectangle(i,i*19,0,18,900);
    }
    for (int i =  0;i < 64;i++) {
        printf("%d : %d\n",i,VDUReadPixel(i*19,12));
    }
}

static void _GraphicsTest2(void) {
    VDUSetGraphicsColour(0,3);
    VDUPlotCommand(4,100,100);
    VDUPlotCommand(5,200,800);
    VDUPlotCommand(5,1100,400);
    VDUPlotCommand(5,100,100);
    VDUSetGraphicsColour(0,1);
    // for (int i = 0;i < 1024;i++) {
    //     VDUPlotCommand(93,440,i);
    // }
    VDUPlotCommand(4,640,512);
    VDUPlotCommand(4,940,512);
    VDUPlotCommand(205,0,612);
}

static void _GraphicsTest3(void) {
    VDUWrite(5);
    VDUSetGraphicsColour(0,129);VDUWrite(16);
    VDUWrite(24);VDUW(80);VDUW(80);VDUW(980);VDUW(880);
    VDUSetGraphicsColour(0,128);VDUWrite(16);
    VDUPlotCommand(4,0,0);
    VDUPlotCommand(5,200,700);
    VDUSetGraphicsColour(0,1);
}

static void _GraphicsTest4(void) {
    VDUWrite(48+mode);
    for (int y = 0;y < 20;y++) {
        if (y % 3 != 2) {
            VDUWrite(31);VDUWrite(0);VDUWrite(y+4);
            for (int x = 0;x < 32;x++) {
                VDUWrite(17);VDUWrite(x+(y %3) * 32);
                VDUWrite(y+64);
            }
        }
    }
    for (int c = 0;c < 64;c++) {
        VDUSetGraphicsColour(0,c);
        int x = (c % 32) * 32;
        int y = 900-(c / 32) * 32 ;
        VDUPlotCommand(4,x,y);
        VDUPlotCommand(101,x+31,y+31);
    }
}

/**
 * @brief      Run the main program
 */
void ApplicationRun(void) {
    int n = 0;
    VDUWrite(22);VDUWrite(mode);                                                    // Switch mode
    _GraphicsTest4();

    SNDCHANNEL c;
    c.type = 0;c.frequency = 440;c.volume = 127;
    SNDUpdate(0,&c);

    while (1) {
  
        n = KBDGetKey();                                                            // Echo any keystroke
        if (n != 0) {                                                               // Handle the CR/LF issue.
            VDUWrite(n);
            if (n == 13) VDUWrite(10);
        }
        if (n == ' ') DemoApp_CheckFileIO();                                        // Dump the USB key on space

        if (n == 'm') {                                                             // Change working mode
            mode=(mode+1) % DVI_MODE_COUNT;
            VDUWrite(22);VDUWrite(mode); 
            struct DVIModeInformation *dmi = DVIGetModeInformation();            
            VDUWriteString("Set mode to %d : %dx%dx%d\r\n",mode,dmi->width,dmi->height,1 << (dmi->bitPlaneCount * dmi->bitPlaneDepth));
            _GraphicsTest4();
        }

        if (KBDEscapePressed(true)) {                                               // Escaped ?
            VDUWriteString("Escape !\r");
        }

        if (HASTICK50_FIRED()) {                                                    // Time to do a 50Hz tick (Don't use this for timing !)
            TICK50_RESET();                                                         // Reset the tick flag
            if (USBUpdate() == 0) return;                                           // Update USB
            KBDCheckTimer();                                                        // Check for keyboard repeat
            
            int x,y,b,w;
            MSEGetState(&x,&y,&b,&w);                                               // Get the mouse state
            if (b != 0) VDUWriteString("Mouse:%d %d %d %d\r",x,y,b,w);              // Show mouse if any button pressed
             
                                        
            CTLState *c = CTLReadController(-1);                                    // Show controller state if anything pressed.
            if (c != NULL) {                                                        // Shouldn't fail as controller -1 uses the keyboard if no gamepad present
                if (c->dx != 0 || c->dy != 0 || c->a || c->b || c->x || c->y) {
                    VDUWriteString("DX:%3d DY:%3d A:%d B:%d X:%d Y:%d\r",c->dx,c->dy,
                                                c->a ? 1:0,c->b ? 1:0,c->x ? 1:0,c->y ? 1:0);
                }
            }
        }
    }
}
