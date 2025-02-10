/**
 * @file       control_codes.h
 *
 * @brief      Header file for Keyboard control codes
 *
 * @author     Paul Robson
 *
 * @date       07/01/2025
 *
 */

#pragma once

#define CONTROL(c)  ((c) & 0x1F)

#define CC_LEFT     136
#define CC_RIGHT    137
#define CC_INSERT   134
#define CC_PAGEDOWN 133
#define CC_END      131
#define CC_BACKSPACE 127
#define CC_TAB      CONTROL('I')
#define CC_LF       CONTROL('J')
#define CC_CLS      CONTROL('L')
#define CC_ENTER    CONTROL('M')
#define CC_PAGEUP   CONTROL('R')
#define CC_DOWN     138
#define CC_HOME     130
#define CC_UP       139
#define CC_DELETE   135
#define CC_ESC      CONTROL('[')

