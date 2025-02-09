/**
 * @file       header.c
 *
 * @brief      Arturo Header Display
 *
 * @author     Paul Robson
 *
 * @date       06/02/2025
 *
 */

#include "common.h"

/**
 * @brief      Display the header.
 */
void HDRDisplay(void) {
    CONWriteString("*** Arturo Kernel [pre Alpha xx-02-25] ***\r\n\r\n");
    CONWriteString("Software by : \r\n");
    CONWriteString("       Lennart Benschop\r\n");
    CONWriteString("       Paul Robson\r\n");
    CONWriteString("Hardware by :\r\n");
    CONWriteString("       Tsvetan Usunov, Olimex\r\n\r\n");
}