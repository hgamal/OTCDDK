#include <avr/io.h>

#include "usart.h"

void USART_Init()
{
	UCSR1A |= (1<<U2X1);

	//unsigned long ubrr = F_CPU / (baud * 16) - 1;
	// See: 
	//		"Atmel-8154-8-bit-AVR-ATmega16A_Datasheet.pdf"
	//		"ATmega16A: 8-bit Microcontroller with 16K Bytes In-System Programmable Flash"
	// 		"19.12 Examples of Baud Rate Setting"
	//		"Table 19-9"
	unsigned int ubrr = 12;	

	/* Set baud rate */
	UBRR1H = (unsigned char)(ubrr>>8);
	UBRR1L = (unsigned char) ubrr;

	/* Enable transmitter */
	UCSR1B = (1<<TXEN1);

	/* Set frame format: 8data, 1stop bit */
	UCSR1C = (1<<UMSEL10)|(3<<UCSZ10);
}

void USART_Transmit(unsigned int data)
{
	/* Wait for empty transmit buffer */
	while (!( UCSR1A & (1<<UDRE1)))
		;

	/* Put data into buffer, sends the data */
	UDR1 = data;
}
