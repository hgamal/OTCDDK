#include <stdio.h>
#include <stdarg.h> 

#include <avr/io.h>

#include "usart.h"
#include "debug.h"

#ifdef XDEBUG

static int printCHAR(char character, FILE *stream)
{
    USART_Transmit(character);
    return 0;
}

static FILE uart_str = FDEV_SETUP_STREAM(printCHAR, NULL, _FDEV_SETUP_RW);

void DEBUG_init()
{
    USART_Init();
    stdout = &uart_str;
}

void DEBUG_msg(const char *msg, ...)
{
    va_list vl;
    va_start(vl, msg);

    vprintf(msg, vl);
}

#endif
