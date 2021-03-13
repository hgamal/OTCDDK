#include <avr/io.h>
#include <util/delay.h>
#include <avr/eeprom.h>

#include "adc.h"
#include "usart.h"
#include "dsp.h"
#include "spi.h"
#include "debug.h"

const char *copyleft = "Haroldo Gamal 2017";

uint16_t EEMEM  eeprom_pot[4];

#define DB_SIZE		4
#define DB_TIME		15
#define DB_DELAY	500

int main (void)
{
	CLKPR = _BV(CLKPCE);
	CLKPR = 0;

    PLLCSR = _BV(PLLE);

    while (PLLCSR & _BV(PLOCK) != 1);

    // PLLFRQ = _BV(PLLTM0) | _BV(PDIV2); // FCLKt4 = 48 MHz
	PLLFRQ = _BV(PLLTM1) | _BV(PLLTM0) | _BV(PDIV2); // FCLKt4 = 24 MHz

    TCCR4A = _BV(COM4A0) | _BV(FOC4A);
    TCCR4B = _BV(CS40);
    TCCR4D = 0;

    TC4H   = 0;
    OCR4A  = 0;
	OCR4C  = 3;

	// F = FCLKt4 / ( 2 * (OCR4C + 1))

    DDRC = _BV(PC7);

    DDRD = _BV(PD5) | _BV(PD6);
	DDRE &= ~_BV(PE2);
           
    while (1) {
		if ((PINE & _BV(PE2)) == 0) {
			PORTD = 0;
			continue;
		}
		PORTD = _BV(PD5);
		_delay_ms(1000);
		PORTD = _BV(PD6);
		_delay_ms(1000);
		PORTD &= ~_BV(PD6);
		_delay_ms(1000);
    }

	DEBUG_init();
	SPI_setup();

	uint16_t delay=1000, oldDelay=1000, currTime=1000;
	uint8_t state_dd2=0, state_dd3=0;

	uint16_t pot[8] = { 0 };

	DDRC |= _BV(PC7) |_BV(PC6);

	DDRD &= ~(_BV(PD2) | _BV(PD3));
	DDRD &= ~_BV(PD0);
	PORTD |= _BV(PORTD2) | _BV(PORTD3);

	ADC_init();

	int pt_index = 0;
	uint16_t value;
	ADC_start(pt_index);

	extern const uint8_t DSP_Code[];

	// DSP_init(DSP_Code, NULL, NULL);

	struct {
		unsigned int dd2:DB_SIZE;
		unsigned int dd3:DB_SIZE;
	} dd;

	int x = 0;
	while(1) {
		if (x == 0) {
			DEBUG_msg("                                                             \r");
			DEBUG_msg("PT1=%d PT2=%d PT3=%d PT4=%d V_CRTL=%d\r", pot[0], pot[1], pot[2], pot[3], pot[7]);
		}
		x = (x+1) % 500;

		if (ADC_isComplete()) {
			value = ADC_read();
			if (value != pot[pt_index]) {
				pot[pt_index] = value;
				//eeprom_write_word (eeprom_pot+pt_index, value);
			}
			pt_index = (pt_index+1) % 8;
			ADC_start(pt_index);
		}

		if (PIND & _BV(PORTD2))
			dd.dd2 = 0;
		else {	// button pressed
			if (dd.dd2 < DB_TIME)
				dd.dd2++;

			if (dd.dd2 == DB_TIME-1) {
				state_dd2 = !state_dd2;
				if (state_dd2) {
					DEBUG_msg("\r\nDD2=ON\r\n");
					PORTC |= _BV(PORTC6);
					PORTD &= ~_BV(PORTD6);
				} else {
					DEBUG_msg("\r\nDD2=OFF\r\n");
					PORTC &= ~_BV(PORTC6);
					PORTD |= _BV(PORTD6);
				}
			}
		}

		if ( PIND & _BV(PORTD3))
			dd.dd3 = 0;
		else {	// button pressed
			if (dd.dd3 < DB_TIME)
				dd.dd3++;

			if (dd.dd3 == DB_TIME-1) {
				state_dd3 = !state_dd3;
				if (state_dd3) {
					DEBUG_msg("\r\nDD3=ON\r\n");
					PORTC &= ~_BV(PORTC7);
				} else {
					DEBUG_msg("\r\nDD3=OFF\r\n");
					PORTC |= _BV(PORTC7);
				}
			}
		}

		_delay_us(DB_DELAY);

	}
}
