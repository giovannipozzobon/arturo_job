/**
 * @file       sound.h
 *
 * @brief      Header file, sound interface
 *
 * @author     Paul Robson
 *
 * @date       07/01/2025
 *
 */

#pragma once

void SNDInitialise(bool _combineChannels);


#define SAMPLE_DIVIDER (32)                 // Divider, affects the interrupts / second of the PWM sample output. More samples are better but slower.

