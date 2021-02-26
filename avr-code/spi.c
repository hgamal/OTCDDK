#include <avr/io.h>
#include "spi.h"
#include "debug.h"

void SPI_setup(void)
{
    // SS + MOSI + SCK = OUTPUT
    DDRB |= (1 << PB2) | (1 << PB5) | (1 << PB7);

    // CPOL =0, CPHA=1, Baud Rate = BusCLK/8 ~= 1.25 MHz
    SPCR = (1 << SPE) | (1 << MSTR) | (1 << CPHA);
    SPSR = (1 << SPI2X);
    // DORD = 0
}

void SPI_sendByte(uint8_t u8Data)
{
    // Load data into the buffer
    SPDR = u8Data;
 
    // Wait until transmission complete
    while(!(SPSR & (1<<SPIF) ))
        ;
}

uint8_t SPI_receiveByte(void)
{	
    //Wait until transmission complete
    while(!(SPSR & (1<<SPIF) ));

    return SPDR;
}

void SPI_sendWord(uint32_t u32Temporal)
{
    SPI_sendByte((uint8_t)(u32Temporal>>16));	// Higher byte of data sent to DSP 
    SPI_sendByte((uint8_t)(u32Temporal>>8));	// Middle byte of data sent to DSP  
    SPI_sendByte((uint8_t)(u32Temporal));		// Lower  byte of data sent to DSP  
}

uint32_t SPI_receiveWord(void)
{
    uint32_t u32ReceivedWord;
    
    u32ReceivedWord  = ((uint32_t)SPI_receiveByte()) <<16;
    u32ReceivedWord += ((uint32_t)SPI_receiveByte()) <<8;    
    u32ReceivedWord += (uint8_t)SPI_receiveByte();

    return(u32ReceivedWord);
}
