#
# MCU Code Makefile by Haroldo Gamal
#

# default FUSE   -U lfuse:w:0x5e:m -U hfuse:w:0x99:m -U efuse:w:0xf3:m 

MCU          = atmega32u4
ARCH         = AVR8
BOARD        = USBKEY
F_CPU        = 8000000
F_USB        = $(F_CPU)
OPTIMIZATION = s
TARGET       = mcu_code
SOURCES      = main.c  # adc.c debug.c dsp.c spi.c usart.c
SRC          = $(SOURCES).c Descriptors.c $(LUFA_SRC_USB) $(LUFA_SRC_USBCLASS)
LUFA_PATH    = lufa/LUFA
CC_FLAGS     = -DUSE_LUFA_CONFIG_HEADER -IConfig/
LD_FLAGS     =

# Include LUFA-specific DMBS extension modules
CCOPTS=-Os -DF_CPU=8000000UL -mmcu=atmega32u4 -DXDEBUG -I ../common
PGMOPT=-B8 -v -pm32u4 -c usbasp -U efuse:w:0xc7:m

# TARGET=mcu-code.hex

all: # ${TARGET}

#OBJS = $(SOURCES:.c=.o)

#ELF = $(basename $(TARGET)).elf

#.c.o:
#	avr-gcc ${CCOPTS} -o $@ -c $<

#$(TARGET):	$(ELF)
#	avr-objcopy -O ihex -R .eeprom $^ $@

#$(ELF):	${OBJS}
#	avr-gcc ${CCOPTS} -Wl,-Map,$(basename $@).map -o $@ $^

#status:
#	avrdude ${PGMOPT}

#program: $(TARGET)
#	avrdude ${PGMOPT} -U flash:w:$(TARGET):i

#dfu: $(TARGET)
#	dfu-programmer $(MCU) erase --force
#	dfu-programmer $(MCU) flash $(TARGET)
	#dfu-programmer $(MCU) reset

#pgmread:
#	avrdude ${PGMOPT} -U flash:r:read.hex:i

#clean:
#	-rm *.map  ${OBJS} $(ELF) ${TARGET}

DMBS_LUFA_PATH ?= $(LUFA_PATH)/Build/LUFA
include $(DMBS_LUFA_PATH)/lufa-sources.mk
include $(DMBS_LUFA_PATH)/lufa-gcc.mk

# Include common DMBS build system modules
DMBS_PATH      ?= $(LUFA_PATH)/Build/DMBS/DMBS
include $(DMBS_PATH)/core.mk
include $(DMBS_PATH)/cppcheck.mk
include $(DMBS_PATH)/doxygen.mk
include $(DMBS_PATH)/dfu.mk
include $(DMBS_PATH)/gcc.mk
include $(DMBS_PATH)/hid.mk
include $(DMBS_PATH)/avrdude.mk
include $(DMBS_PATH)/atprogram.mk
