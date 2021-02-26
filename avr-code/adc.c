#include <avr/io.h>

#include "adc.h"

void ADC_init()
{
	// AVCC with external capacitor at AREF pin
    ADMUX=(1<<REFS0);

	//Rrescalar div factor = 128
	ADCSRA=(1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
}

void ADC_start(uint8_t ch)
{
	ch=ch & 0b00000111;
	ADMUX = (ADMUX & ~7) | ch;

	//Start Single conversion
	ADCSRA|=(1<<ADSC);
}

uint8_t ADC_isComplete()
{
	return ADCSRA & (1<<ADIF);
}

uint16_t ADC_read()
{
	ADCSRA|=(1<<ADIF);

	return(ADC);
}

uint16_t start_readChannel(uint8_t ch)
{
	ADC_start(ch);

	//Wait for conversion to complete
	while(!ADC_isComplete());

	ADC_read();
}

