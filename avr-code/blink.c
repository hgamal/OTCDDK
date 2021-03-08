#include <avr/io.h>
#include <util/delay.h>

int main (void)
{
        DDRD = _BV(PD5) | _BV(PD6);

        while (1) {
                PORTD = _BV(PD5);
                _delay_ms(100);
                PORTD = _BV(PD6);
                _delay_ms(100);
		PORTD &= ~_BV(PD6);
                _delay_ms(500);
        }
}
