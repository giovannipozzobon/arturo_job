/**
 * @file       system.h
 *
 * @brief      System configuration
 *
 * @author     Paul Robson
 *
 * @date       06/02/2025
 *
 */

#pragma once

//
// 		Extra SRAM is used if we support 640x480x8 mode.
//
#if DVI_SUPPORT_640_480_8 == 0
#define EXTRA_SRAM_USED 	(0)
#else
#define EXTRA_SRAM_USED 	(56)
#endif
//
//		Available SRAM in kilobytes.
//
#define SRAM_AVAILABLE 		(160-EXTRA_SRAM_USED)
//
//      Sound hardware setup.
//
#if PICO_HARDWARE==PICO_HW_RP2040PC
#define AUDIO_HARDWARE_CHANNELS     (2)
#define AUDIO_PIN_L                 (20) 
#define AUDIO_PIN_R                 (21) 
#elif PICO_HARDWARE==PIC_HW_NEO6502
#define AUDIO_HARDWARE_CHANNELS     (1)
#define AUDIO_PIN_L                 (20) 
#define AUDIO_PIN_R                 (-1)
#else
#error("Unsupported hardware")
#endif

