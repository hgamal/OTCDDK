#include <avr/io.h>
#include <util/delay.h>

int main (void)
{
	uint16_t delay=1000, oldDelay=1000, currTime=1000;
	uint8_t state=0;

	DDRC |= _BV(PC1) |_BV(PC0);
	#DDRD &= ~_BV(PD2);
	#PORTD |= _BV(PORTD2);

	while(1) {
		if (state & 1)
			PORTC &= ~_BV(PORTC0);
		else
			PORTC |= _BV(PORTC0);

		//if (currTime == 0) {
		//	state = !state;
		;;	currTime = delay;
		//}

		state ++;
		currTime--;
	
		_delay_us(20);

	}
}
