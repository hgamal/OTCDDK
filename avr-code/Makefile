#
# MCU Code Makefile by Haroldo Gamal
#

# default HFUSE: avrdude -c usbasp -pm16 -v -B16 -U lfuse:w:0x99:m
# default LFUSE: avrdude -c usbasp -pm16 -v -B16 -U lfuse:w:0xe1:m

CCOPTS=-Os -DF_CPU=1000000UL -mmcu=atmega16 -DXDEBUG -I ../common

PGMOPT=-B8 -v -pm16 -c usbasp

TARGET=mcu-code.hex

all: ${TARGET}

SOURCES = main.c adc.c debug.c dsp.c spi.c usart.c TwoBandStereoEQ-DSPCode.c

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

pgmread:
	avrdude ${PGMOPT} -U flash:r:read.hex:i

clean:
	-rm *.map  ${OBJS} $(ELF) ${TARGET}
