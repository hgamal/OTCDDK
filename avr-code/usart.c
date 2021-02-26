#include <avr/io.h>

#include "usart.h"

void USART_Init()
{
	UCSRA |= (1<<U2X);

	//unsigned long ubrr = F_CPU / (baud * 16) - 1;
	// See: 
	//		"Atmel-8154-8-bit-AVR-ATmega16A_Datasheet.pdf"
	//		"ATmega16A: 8-bit Microcontroller with 16K Bytes In-System Programmable Flash"
	// 		"19.12 Examples of Baud Rate Setting"
	//		"Table 19-9"
	unsigned int ubrr = 12;	

	/* Set baud rate */
	UBRRH = (unsigned char)(ubrr>>8);
	UBRRL = (unsigned char) ubrr;

	/* Enable transmitter */
	UCSRB = (1<<TXEN);

	/* Set frame format: 8data, 1stop bit */
	UCSRC = (1<<URSEL)|(3<<UCSZ0);
}

void USART_Transmit(unsigned int data)
{
	/* Wait for empty transmit buffer */
	while (!( UCSRA & (1<<UDRE)))
		;

	/* Put data into buffer, sends the data */
	UDR = data;
}
