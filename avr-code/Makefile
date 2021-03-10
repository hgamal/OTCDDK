#
# MCU Code Makefile by Haroldo Gamal
#

# default FUSE   -U lfuse:w:0x5e:m -U hfuse:w:0x99:m -U efuse:w:0xf3:m 

CCOPTS=-Os -DF_CPU=1000000UL -mmcu=atmega32u4 -DXDEBUG -I ../common

PGMOPT=-B8 -v -pm32u4 -c usbasp -U efuse:w:0xc7:m

MCU=atmega32u4

TARGET=mcu-code.hex

all: ${TARGET}

SOURCES = main.c  # adc.c debug.c dsp.c spi.c usart.c

OBJS = $(SOURCES:.c=.o)

ELF = $(basename $(TARGET)).elf

.c.o:
	avr-gcc ${CCOPTS} -o $@ -c $<

$(TARGET):	$(ELF)
	avr-objcopy -O ihex -R .eeprom $^ $@

$(ELF):	${OBJS}
	avr-gcc ${CCOPTS} -Wl,-Map,$(basename $@).map -o $@ $^

status:
	avrdude ${PGMOPT}

program: $(TARGET)
	avrdude ${PGMOPT} -U flash:w:$(TARGET):i

dfu: $(TARGET)
	dfu-programmer $(MCU) erase --force
	dfu-programmer $(MCU) flash $(TARGET)
	#dfu-programmer $(MCU) reset

pgmread:
	avrdude ${PGMOPT} -U flash:r:read.hex:i

clean:
	-rm *.map  ${OBJS} $(ELF) ${TARGET}
