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
	DEBUG_init();
	SPI_setup();

	uint16_t delay=1000, oldDelay=1000, currTime=1000;
	uint8_t state_dd2=0, state_dd3=0;

	uint16_t pot[8] = { 0 };

	DDRC |= _BV(PC1) |_BV(PC0);

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
					PORTC |= _BV(PORTC0);
					PORTD &= ~_BV(PORTD0);
				} else {
					DEBUG_msg("\r\nDD2=OFF\r\n");
					PORTC &= ~_BV(PORTC0);
					PORTD |= _BV(PORTD0);
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
					PORTC &= ~_BV(PORTC1);
				} else {
					DEBUG_msg("\r\nDD3=OFF\r\n");
					PORTC |= _BV(PORTC1);
				}
			}
		}

		_delay_us(DB_DELAY);

	}
}
