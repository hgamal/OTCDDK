#ifndef _SPI_H
#define _SPI_H

void     SPI_setup(void);
void     SPI_sendByte(uint8_t u8Data);
uint8_t  SPI_receiveByte(void);

void     SPI_sendWord(uint32_t u32Temporal);
uint32_t SPI_receiveWord(void);

#endif
