#include <avr/io.h>
#include <util/delay.h>

#include "spi.h"
#include "debug.h"

/* 
   This is the compiled code for the DSP that expands the DSP program memory to 1.25k
*/
const uint8_t codeExpandDSP_RAM[] = 
{
    0x0A ,0xFA, 0x67,     
    0x0A, 0xF0, 0x80, 
    0xFF, 0x00, 0x00
};

static void DSP_reset()
{
    DEBUG_msg("DSP_reset\r\n");

    // set reset line down for min 500 ns min
    PORTB |= _BV(PB0);
    _delay_us(1);
    PORTB &= ~_BV(PB0);
}

static uint8_t DSP_isHreqReady()
{
    return (PINB & _BV(PINB1)) == 0;
}

static void DSP_waitHreqReady()
{
    while(!DSP_isHreqReady())
        DEBUG_msg("HREQ + %02x\r", PINB & _BV(PINB1));
}

static void DSP_setSS()
{
    DEBUG_msg("DSP_setSS\r\n");

    PINB &= ~_BV(PINB2);
}

static void DSP_resetSS()
{
    DEBUG_msg("DSP_resetSS\r\n");

    PINB |= _BV(PINB2);
}

static void DSP_sendCode(const uint8_t *codeBuffer, uint32_t dspSize, uint32_t startAddr)
{
   // Wait DSP to be ready
    DSP_waitHreqReady();

    DSP_setSS();

    const uint8_t *pDSPCode = codeBuffer;

    DEBUG_msg("send code size=%d\r\n", dspSize);

    // Send DSP expanding RAM code size
    SPI_sendWord(dspSize);

    DSP_waitHreqReady();

    DEBUG_msg("send code start=%x\r\n", startAddr);
    
     // Send DSP expanding RAM code start address
    SPI_sendWord(startAddr);

    // Send DSP code
    while(dspSize) {
        DSP_waitHreqReady();

        uint32_t data;

        data  = ((uint32_t) (*pDSPCode)) << 16;
        SPI_sendByte(*pDSPCode++);

        data += ((uint32_t) (*pDSPCode)) << 8;
        SPI_sendByte(*pDSPCode++);

        data += ((uint32_t) (*pDSPCode));
        SPI_sendByte(*pDSPCode++);

        DEBUG_msg("Send DSP %08x: %08x\r", startAddr, data);

        startAddr++;
        dspSize--;
    }
  
    DSP_resetSS();

}

static void DSP_upload(const uint8_t *codeBuffer, const uint8_t *xBuffer, const uint8_t *yBuffer)
{
    // Reset DSP
    DSP_reset();

    uint32_t dspSize = sizeof(codeExpandDSP_RAM)/3;

    DSP_sendCode(codeExpandDSP_RAM, dspSize, 0);

    const uint8_t *pDSPCode = codeBuffer;

    // get code size from buffer
    dspSize  = ((uint32_t) (*pDSPCode++)) << 16;
    dspSize += ((uint32_t) (*pDSPCode++)) << 8;
    dspSize += ((uint32_t) (*pDSPCode++));
    
    // get DSP code start
    uint32_t dspCodeStart;
    dspCodeStart  = ((uint32_t) (*pDSPCode++)) << 16;
    dspCodeStart += ((uint32_t) (*pDSPCode++)) << 8;
    dspCodeStart += ((uint32_t) (*pDSPCode++));

    DSP_sendCode(codeExpandDSP_RAM, dspSize, 0);

    // wait 50 ms for DSP start
    _delay_ms(50);
}

void DSP_init(const uint8_t *codeBuffer, const uint8_t *xBuffer, const uint8_t *yBuffer)
{
    DSP_upload(codeBuffer, xBuffer, yBuffer);
}