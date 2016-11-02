#--------------------------------------------------------------------------------------
# File:    Makefile for an AVR project
#          The makefile is the standard way to control the compilation and linking of 
#          C/C++ files into an executable file. This makefile is also used to control
#          the downloading of the executable file to the target processor and the 
#          generation of documentation for the project. 
#
# Version:  4-11-2004 JRR Original file
#           6-19-2006 JRR Modified to use AVR-JTAG-ICE for debugging
#          11-21-2008 JRR Added memory locations and removed extras for bootloader
#          11-26-2008 JRR Cleaned up; changed method of choosing programming method
#
# Relies   The avr-gcc compiler and avr-libc library
# on:      The avrdude downloader, if downloading through an ISP port
#          AVR-Insight or DDD and avarice, if debugging with the JTAG port
#          Doxygen, for automatic documentation generation
#
# Copyright 2006-2008 by JR Ridgely.  This makefile is intended for use in educational 
# courses only, but its use is not restricted thereto. It is released under the terms 
# of the Lesser GNU Public License with no warranty whatsoever. Anyone who uses this 
# file agrees to take full responsibility for any and all consequences of that use. 
#--------------------------------------------------------------------------------------

# The name of the program you're building, and the list of object files. There must be
# an object file listed here for each *.c or *.cc file in your project. The make
# program will automatically figure out how to compile and link your C or C++ files 
# from the list of object files. TARGET will be the name of the downloadable program.

TARGET = timescape
OBJS = $(TARGET).o  base_text_serial.o rs232.o avr_adc.o stl_timer.o stl_task.o stepper.o intervelometer.o lcd.o micromenu.o lcdmenu1.o menu.o task_menu.o task_navigation.o 
				
# Clock frequency of the CPU, in Hz. This number should be an unsigned long integer.
# For example, 16 MHz would be represented as 16000000UL. For ME405 boards, clocks are
# usually 4MHz for Ministrone and Swoop sensor boards, 8MHz for single-motor ME405 
# boards, and 16MHz for dual-motor ME405 boards
CKF = 16000000UL

# These codes are used to switch on debugging modes if they're being used. Several can
# be placed on the same line together to activate multiple debugging tricks at once.
# -DSTL_SERIAL_DEBUG   For general debugging through a serial device
# -DSTL_SERIAL_TRACE   For printing state transition traces on a serial device
DBG = -DSTL_SERIAL_DEBUG

# This define is used to choose the type of programmer from the following options: 
# bsd        - Parallel port in-system (ISP) programmer using SPI interface on AVR
# jtagice    - Serial or USB interface JTAG-ICE mk I clone from ETT or Olimex
# bootloader - Resident program in the AVR which downloads through USB/serial port
# PROG = bsd
# PROG = jtagice
# PROG = bootloader
PROG = avrispmkII

# These defines specify the ports to which the downloader device is connected. 
# PPORT is for "bsd" on a parallel port, lpt1 on Windows or /dev/parport0 on Linux.
# JPORT is for "jtagice" on a serial port com1 or /dev/ttyS0, or usb-serial such as
#       com4 or /dev/ttyUSB1, or aliased serial port such as /dev/avrjtag
# BPORT is for "bootloader", the USB/serial port program downloader on the AVR
# PPORT = /dev/parport0
PPORT = lpt3
JPORT = /dev/ttyUSB1
BPORT = /dev/ttyUSB0
APORT = usb

#--------------------------------------------------------------------------------------
# This section specifies the type of CPU; uncomment the section for your processor. 
# Fuse bytes are specified for standard debugging use (STD_...) and low power use with
# the JTAG interface disabled (LOP_...).

# ATmega128/L: Used on the ME405 boards with single and dual motor controllers
# Bootloader at 0x1F000, standard fuses FF11EF, bootloader fuses FF12EF
# These values leave the JTAG interface enabled (disable it to save power)
 #CHIP = m128
 #MCU = atmega128
 #STD_EFUSE = 0xFF
 #STD_HFUSE = 0x11
 #STD_LFUSE = 0xEF

# ATmega32: 
# CHIP = m32
# MCU = atmega32
# STD_HFUSE = 0x19
# STD_LFUSE = 0xEF

# ATmega324P/PV: Used on the Swoop sensor board. Note JTAG-ICE-I won't work with it.
# Bootloader at 0x7800, standard fuses FF19EF, bootloader fuses FFC8EF
# Low power fuses FF11EF
# CHIP = m324p
# MCU = atmega324p
# STD_EFUSE = 0xFF
# STD_HFUSE = 0x11
# STD_LFUSE = 0xEF
# LOP_EFUSE = 0xFF
# LOP_HFUSE = 0x11
# LOP_LFUSE = 0xEF

# ATmega644 or ATmega644V (not ATmega644P)
# CHIP = m644
# MCU = atmega644

# ATmega8 on small Ministrone sensor boards
# CHIP = m8
# MCU = atmega8

# CHIP = 2313
# MCU = at90s2313

# CHIP = m8535
# MCU = atmega8535

#ATmega168 : Used for Arduino Duemilanove Board 
#Fuses should read 0xF8 0xDF 0xFF
#    * Boot Flash section size = 1024 words Boot start address = $1C00; [BOOTSZ=00]; default value
#    * Boot Reset Vector Enabled (default address = $0000); [BOOTRST=0]
#    * Brown-out detection disabled; [BODLEVEL=111]
#    * Ext. Crystal Osc.; Frequency 8.0 - MHz; Startup time PWRDWN/RESET: 16K CK/14 CK + 64ms; [CKSEL=1111 SUT=0]
#  CHIP = m168
#  MCU = atmega168
#  STD_EFUSE = 0xF8
#  STD_HFUSE = 0xDF
#  STD_LFUSE = 0xFF
  
#ATmega2560 : Used for Arduino MEGA 2560 V3 Board 
#FUSES should give you:
#	*Boot Reset vector Enabled (default address=$0000); [BOOTRST=0]
#	*Ext Crystal Osc. Frew 8.0MHz; startup time: 16K CK + 65ms; [CKSEL=1111 SUT=11]
#	*Serial program downloading (SPI) enabled; [SPIEN=0]
#	SPIEN
#	BOOTSZ1
#	BOOTRST
#	BODLEVEL1
  CHIP = m2560
  MCU = atmega2560
  STD_EFUSE = 0xF5
  STD_HFUSE = 0xDA
  STD_LFUSE = 0xFF

#--------------------------------------------------------------------------------------
# Define which compiler to use (CC) and some standard compiler options (STD).
CC = avr-gcc
STD = _GNU_SOURCE

# Usually this is just left at debugging level 0
DEBUGL = DEBUG_LEVEL=0

# Tell the compiler how hard to try to optimize the code. Optimization levels are:
# -O0  Don't try to optimize anything (even leaves empty delay loops in)
# -O1  Some optimizations; code usually smaller and faster than O0
# -O2  Pretty high level of optimization; often good compromise of speed and size
# -O3  Tries really hard to make code run fast, even if code size gets pretty big
# -Os  Tries to make code size small. Sometimes -O1 makes it smaller, though(?!)
OPTIM = -O2

# The JTAG bitrate and IP port are used to configure the JTAG-ICE debugger
JTAGBITRATE = 1000000
IPPORT = :4242

# Any other compiler switches go here, for example short enumerations to save memory
OTHERS = -fshort-enums

#--------------------------------------------------------------------------------------
# Inference rules show how to process each kind of file. 

# How to compile a .c file into a .o file
.c.o:
	$(CC) -c -g $(OPTIM) -mmcu=$(MCU) -DCPU_FREQ_Hz=$(CKF) $(DBG) $(OTHERS) $<

# How to compile a .cc file into a .o file
.cc.o:
	$(CC) -c -g $(OPTIM) -mmcu=$(MCU) -DCPU_FREQ_Hz=$(CKF) $(DBG) $(OTHERS) $<

# This rule controls the linking of the target program from object files. The target 
# is saved as an ELF debuggable binary, downloadable .hex, and raw binary .bin.  Also
# created is a listing file .lst, which shows generated machine and assembly code.
$(TARGET).elf:  $(OBJS)
	$(CC) $(OBJS) -g -mmcu=$(MCU) -o $(TARGET).elf
	@avr-objdump -h -S $(TARGET).elf > $(TARGET).lst
	@avr-objcopy -j .text -j .data -O ihex $(TARGET).elf $(TARGET).hex
	@avr-objcopy -j .text -j .data -O binary $(TARGET).elf $(TARGET).bin
	@ls -l $(TARGET).bin

#--------------------------------------------------------------------------------------
# Make the main file of this project.  This target is invoked when the user just types 
# 'make' as opposed to 'make <target>.'  This should be the first target in Makefile.

all:  $(TARGET).elf

#--------------------------------------------------------------------------------------
# This is a dummy target that doesn't do anything. It's included because the author 
# belongs to a faculty labor union and has been instilled with reverence for laziness.

nothing:

#--------------------------------------------------------------------------------------
# 'make install' will make the project, then download the program using whichever 
# method has been selected -- ISP cable, JTAG-ICE module, or USB/serial bootloader

install:  $(TARGET).elf
  ifeq ($(PROG), bsd)
	avrdude -p $(CHIP) -P $(PPORT) -c bsd -V -E noreset \
	-Uflash:w:$(TARGET).hex
  else ifeq ($(PROG), avrispmkII)
	avrdude -p $(CHIP) -P $(APORT) -c avrispmkII -E noreset \
	-Uflash:w:$(TARGET).hex
  else ifeq ($(PROG), jtagice)
	avarice -e -p -f $(TARGET).elf -j $(JPORT)
  else ifeq ($(PROG), bootloader)
	ruby bootloader.rb $(BPORT) $(TARGET).bin
  else
	@echo "ERROR: No programmer" $(PROG) "in the Makefile"
  endif

#--------------------------------------------------------------------------------------
# 'make debug' will make the project, then download it with the JTAG-ICE-I interface
# and start up a debugger. The JTAG-ICE-I only works with the ATmega32, ATmega128, and
# a few other somewhat older processors; it won't work with the ATmega324/644.

debug:  $(TARGET).elf $(GDBINITFILE)
	@avarice --capture -B $(JTAGBITRATE) -j $(JPORT) $(IPPORT) &
	@sleep 1
	@ddd --debugger "avr-gdb -x $(GDBINITFILE)"
	@killall -q avarice || /bin/true

$(GDBINITFILE): $(TARGET).hex
	@echo "file $(TARGET).elf"            > $(GDBINITFILE)
	@echo "target remote localhost:4242" >> $(GDBINITFILE)
	@echo "load"                         >> $(GDBINITFILE) 
	@echo "break main"                   >> $(GDBINITFILE)
	@echo "continue"                     >> $(GDBINITFILE)

#--------------------------------------------------------------------------------------
# 'make term' will run the serial port emulator GTK-Term, a hacked version of which
# can be used to download programs to the bootloader with the ctrl-D command.

term:  $(TARGET).elf
	@gtkterm --to-send $(TARGET).bin &

#--------------------------------------------------------------------------------------
# 'make fuses' will set up the processor's fuse bits in a "standard" mode. Standard is
# a setup in which there is no bootloader but the ISP and JTAG interfaces are enabled. 

fuses: nothing
  ifeq ($(PROG), bsd)
	avrdude -p $(CHIP) -P $(PPORT) -c $(PROG) -V -E noreset -Ulfuse:w:$(STD_LFUSE):m 
	avrdude -p $(CHIP) -P $(PPORT) -c $(PROG) -V -E noreset -Uhfuse:w:$(STD_HFUSE):m 
	avrdude -p $(CHIP) -P $(PPORT) -c $(PROG) -V -E noreset -Uefuse:w:$(STD_EFUSE):m
  else ifeq ($(PROG), jtagice)
	avarice -e -j $(JPORT) --write-fuses FF11EF
  else
	@echo "ERROR: Only bsd/ISP or JTAG-ICE can program fuse bytes"
  endif

#--------------------------------------------------------------------------------------
# 'make readfuses' will see what the fuses currently hold

readfuses: nothing
  ifeq ($(PROG), bsd)
	@echo "ERROR: Not yet programmed to read fuses with bsd/ISP cable"
  else ifeq ($(PROG), jtagice)
	@avarice -e -j $(JPORT) --read-fuses
  else ifeq ($(PROG), bootloader)
	@echo "ERROR: Not yet programmed to read fuses via bootloader"
  else
	@echo "ERROR: No known device specified to read fuses"
  endif

#-----------------------------------------------------------------------------
# 'make reset' will read a byte of lock bits, ignore it, and reset the chip
# This can be used to un-freeze the chip after a 'make freeze'
reset:
  ifeq ($(PROG), bsd)
	avrdude -c bsd -p $(CHIP) -P $(PPORT) -c $(PROG) -V -E noreset \
		-Ulfuse:r:/dev/null:r
  else
	@echo "ERROR: make reset only works with parallel ISP cable"
  endif

#-----------------------------------------------------------------------------
# 'make freeze' will read a byte of lock bits and leave the chip in reset
freeze:
  ifeq ($(PROG), bsd)
	avrdude -c bsd -p $(CHIP) -P $(PPORT) -c $(PROG) -V -E reset \
		-Ulfuse:r:/dev/null:r
  else
	@echo "ERROR: make freeze only works with parallel ISP cable"
  endif

#--------------------------------------------------------------------------------------
# 'make doc' will use Doxygen to create documentation for the project.

doc:  $(TARGET).elf
	doxygen doxygen.conf

#--------------------------------------------------------------------------------------
# 'make clean' will erase the compiled files, listing files, etc. so you can
# restart the building process from a clean slate.

clean:
	rm -f *.o $(TARGET).hex $(TARGET).lst $(TARGET).elf $(TARGET).bin
	rm -fr html rtf *~

#-----------------------------------------------------------------------------
# 'make help' will show a list of things this makefile can do

help:
	@echo 'make          - Build program file ready to download'
	@echo 'make install  - Build program and download with parallel ISP cable'
	@echo 'make debug    - Build program and debug using JTAG-ICE module'
	@echo 'make freeze   - Stop processor with parallel cable RESET line'
	@echo 'make reset    - Reset processor with parallel cable RESET line'
	@echo 'make doc      - Generate documentation with Doxygen'
	@echo 'make clean    - Remove compiled files; use before archiving files'
	@echo ' '
	@echo 'Notes: 1. Other less commonly used targets are in the Makefile'
	@echo '       2. You can combine targets, as in "make clean debug"'
