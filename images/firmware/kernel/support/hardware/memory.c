/**
 * @file       memory.c
 *
 * @brief      Memory handler
 *
 * @author     Paul Robson
 *
 * @date       06/02/2025
 *
 */


#include "common.h"

//
//      This will not be defined for the simulator, so we allocate 256k.
//
#ifndef SRAM_AVAILABLE
#define SRAM_AVAILABLE  (256)
#endif

//
//      The system memory.
//
uint8_t  systemMemory[SRAM_AVAILABLE * 1024];

/**
 * @brief      Return size of available SRAM in the system
 *
 * @return     size of SRAM in bytes
 */
int MEMGetMemorySize(void) {
    return sizeof(systemMemory);
}
/**
 * @brief      Get the base address of system memory
 *
 * @return     Pointer to the base address.
 */
uint8_t *MEMGetMemory(void) {
    return systemMemory;
}