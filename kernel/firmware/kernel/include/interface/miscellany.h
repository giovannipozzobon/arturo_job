/**
 * @file       miscellany.h
 *
 * @brief      Header file, miscellany
 *
 * @author     Paul Robson
 *
 * @date       07/01/2025
 *
 */

#pragma once
//
//      Main execution
//
void ApplicationRun(void);
//
//      Timer/Interrupt functions.
//
int TMRReadTimeMS(void);
//
//      Timer Yield function
//
bool SYSYield(void);
//
//		App still running.
//
bool SYSAppRunning(void);