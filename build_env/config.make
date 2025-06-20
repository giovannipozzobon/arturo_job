# *******************************************************************************************
# *******************************************************************************************
#
#       Name :      config.make
#       Purpose :   Configuration filess
#       Date :      18th December 2024
#       Author :    Paul Robson (paul@robsons.org.uk)
#
# *******************************************************************************************
# *******************************************************************************************

# *******************************************************************************************
#
#                                       Configuration
#
# *******************************************************************************************
#
#       Set these accordingly. 
#
export PICO_SDK_PATH := /Users/giovanni.pozzobon/Documents/pico/pico-sdk
export PICO_DVI_PATH := /Users/giovanni.pozzobon/Documents/GitHub/rp2040_pico_job/PicoDVI
#
#       Directory the App is in.
#
ARTURO_APP_DIRECTORY = $(APPDIR)/default
#
#       Configure Platform (rp2040,rp2350,rp2350-riscv)
#
PLATFORM = rp2040
#
#       Specifies the physical hardware used (PICO_HW_RP2040PC,PICO_HW_NEO6502)
#
PICO_HARDWARE = PICO_HW_RP2040PC
#
#       If non-zero, USB keyboard messages are converted to ASCII and Localised. It also allows
#       the state of each key (up or down) to be read. If zero, then the responsibility for 
#       handling USB Key events lies with the author.
#
ARTURO_PROCESS_KEYS = 1
#
#       If non-zero, Console write is passed to the VDU Handler (e.g. it calls VDUWrite), if zero
#       the developer needs to provide their own CONWrite for handling debug and other instructions.
#
ARTURO_PROCESS_CONSOLE = 1
#
#       If non-zero, use mono sound (e.g. combine the two RP2040PC channels into one)
#
ARTURO_MONO_SOUND = 1
#
#       If non-zero, use the sound library
#
ARTURO_PROCESS_SOUND = 1
#
#       Set the default locale, the list is in the locale directory
#
ARTURO_KBD_LOCALE = "gb"
#
#       Allow the 640x480x8 mode. This requires 115200 bytes of the RAM - about half of it
#       so this may well exclude stock RP2040
#
DVI_SUPPORT_640_480_8 = 0

# *******************************************************************************************
#
#                                     OpenOCD Configuration
#
# *******************************************************************************************
#
#       OpenOCD must be build as per "Getting started with Raspberry PI Pico" Appendix A
#       Most distro builds currently won't work.
#
UPLOADER = openocd 
UPCONFIG = -f interface/cmsis-dap.cfg -f target/$(PLATFORM).cfg 
UPCOMMANDS = -c "adapter speed 5000" -c "program $(BINDIR)$(PROJECT).elf verify reset exit"

# *******************************************************************************************
#
#                                     Serial Configuration
#
# *******************************************************************************************

PICO_SERIAL_PORT = /dev/ttyUSB0
PICO_SERIAL_BAUD_RATE = 115200

# *******************************************************************************************
#
#                   Used to output the data to __config.h. 
#
# *******************************************************************************************

#
#       DO NOT MANUALLY CHANGE THESE ! Must be a better way of doing this reliably.
#
CONFIG_DATA="\
\#define PICO_PLATFORM_NAME "$(PLATFORM)" |\
\
\#define PICO_HW_RP2040PC     (0) |\
\#define PICO_HW_NEO6502      (1) |\
\#define PICO_HARDWARE        $(PICO_HARDWARE)  |\
\
\#define ARTURO_PROCESS_KEYS    $(ARTURO_PROCESS_KEYS)          |\
\#define ARTURO_PROCESS_CONSOLE $(ARTURO_PROCESS_CONSOLE)       |\
\#define ARTURO_PROCESS_SOUND   $(ARTURO_PROCESS_SOUND)         |\
\#define ARTURO_MONO_SOUND      $(ARTURO_MONO_SOUND)            |\
\#define ARTURO_KBD_LOCALE      $(ARTURO_KBD_LOCALE)            |\
\#define DVI_SUPPORT_640_480_8  $(DVI_SUPPORT_640_480_8)        |\
"
