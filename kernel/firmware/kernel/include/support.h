/**
 * @file       support.h
 *
 * @brief      Header file composite for support section
 *
 * @author     Paul Robson
 *
 * @date       07/01/2025
 *
 */


#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>
#if defined(__linux__) || defined(_WIN32)
  #include <malloc.h>
#else
  #include <stdlib.h>
#endif

#include "support/memory.h"
#include "support/kbd_codes.h"
#include "support/console.h"
#include "support/miscellany.h"
#include "support/keyboard.h"
#include "support/control_codes.h"
#include "support/fileio.h"
#include "support/soundsystem.h"
#include "support/vdu.h"
