#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/power.h>
#include <avr/interrupt.h>
#include <string.h>
#include <stdio.h>
#include <util/delay.h>
#include <avr/eeprom.h>

#include "descriptors.h"

#include <LUFA/Drivers/Board/LEDs.h>
#include <LUFA/Drivers/Board/Joystick.h>
#include <LUFA/Drivers/USB/USB.h>
#include <LUFA/Platform/Platform.h>

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

static FILE USBSerialStream;

uint8_t connected = false;

USB_ClassInfo_CDC_Device_t VirtualSerial_CDC_Interface =
	{
		.Config =
			{
				.ControlInterfaceNumber   = INTERFACE_ID_CDC_CCI,
				.DataINEndpoint           =
					{
						.Address          = CDC_TX_EPADDR,
						.Size             = CDC_TXRX_EPSIZE,
						.Banks            = 1,
					},
				.DataOUTEndpoint =
					{
						.Address          = CDC_RX_EPADDR,
						.Size             = CDC_TXRX_EPSIZE,
						.Banks            = 1,
					},
				.NotificationEndpoint =
					{
						.Address          = CDC_NOTIFICATION_EPADDR,
						.Size             = CDC_NOTIFICATION_EPSIZE,
						.Banks            = 1,
					},
			},
	};

/* Function Prototypes: */
void SetupHardware(void);
void CheckJoystickMovement(void);

void EVENT_USB_Device_Connect(void);
void EVENT_USB_Device_Disconnect(void);
void EVENT_USB_Device_ConfigurationChanged(void);
void EVENT_USB_Device_ControlRequest(void);

int main (void)
{
	SetupHardware();

	/* Create a regular character stream for the interface so that it can be used with the stdio.h functions */
	CDC_Device_CreateStream(&VirtualSerial_CDC_Interface, &USBSerialStream);

	//LEDs_SetAllLEDs(LEDMASK_USB_NOTREADY);
	GlobalInterruptEnable();
           
    while (1) {
		if ((PINE & _BV(PE2)) == 0) {
			PORTD = 0;
			continue;
		}
		PORTD = _BV(PD5);
		_delay_ms(100);
		PORTD = _BV(PD6);
		_delay_ms(100);
		PORTD &= ~_BV(PD6);
		_delay_ms(100);

		static uint32_t counter = 0;

		if (connected) {
			/* Write the string to the virtual COM port via the created character stream */
			fprintf(&USBSerialStream, "%ld\r", counter++);

			/* Alternatively, without the stream: */
			// CDC_Device_SendString(&VirtualSerial_CDC_Interface, ReportString);

			/* Must throw away unused bytes from the host, or it will lock up while waiting for the device */
			CDC_Device_ReceiveByte(&VirtualSerial_CDC_Interface);
		}

		CDC_Device_USBTask(&VirtualSerial_CDC_Interface);
		USB_USBTask();
    }

	DEBUG_init();
	SPI_setup();

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

	// extern const uint8_t DSP_Code[];

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

void SetupHardware(void)
{
	CLKPR = _BV(CLKPCE);
	CLKPR = 0;

    PLLCSR = _BV(PLLE);

    while ((PLLCSR & _BV(PLOCK)) != 1);

    PLLFRQ = _BV(PLLTM0) | _BV(PDIV2); // FCLKt4 = 48 MHz
	//PLLFRQ = _BV(PLLTM1) | _BV(PLLTM0) | _BV(PDIV2); // FCLKt4 = 24 MHz

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

	/* Disable watchdog if enabled by bootloader/fuses */
	MCUSR &= ~(1 << WDRF);
	wdt_disable();

	/* Disable clock division */
	clock_prescale_set(clock_div_1);

	/* Hardware Initialization */
	USB_Init();
}

/** Event handler for the library USB Connection event. */
void EVENT_USB_Device_Connect(void)
{
	connected = true;
	//LEDs_SetAllLEDs(LEDMASK_USB_ENUMERATING);
}

/** Event handler for the library USB Disconnection event. */
void EVENT_USB_Device_Disconnect(void)
{
	//connected = false;
	//LEDs_SetAllLEDs(LEDMASK_USB_NOTREADY);
}

/** Event handler for the library USB Configuration Changed event. */
void EVENT_USB_Device_ConfigurationChanged(void)
{
	bool ConfigSuccess = true;
	ConfigSuccess &= CDC_Device_ConfigureEndpoints(&VirtualSerial_CDC_Interface);

	//LEDs_SetAllLEDs(ConfigSuccess ? LEDMASK_USB_READY : LEDMASK_USB_ERROR);
	//connected = ConfigSuccess;
}

/** Event handler for the library USB Control Request reception event. */
void EVENT_USB_Device_ControlRequest(void)
{
	CDC_Device_ProcessControlRequest(&VirtualSerial_CDC_Interface);
}

/** CDC class driver callback function the processing of changes to the virtual
 *  control lines sent from the host..
 *
 *  \param[in] CDCInterfaceInfo  Pointer to the CDC class interface configuration structure being referenced
 */
void EVENT_CDC_Device_ControLineStateChanged(USB_ClassInfo_CDC_Device_t *const CDCInterfaceInfo)
{
	/* You can get changes to the virtual CDC lines in this callback; a common
	   use-case is to use the Data Terminal Ready (DTR) flag to enable and
	   disable CDC communications in your application when set to avoid the
	   application blocking while waiting for a host to become ready and read
	   in the pending data from the USB endpoints.
	*/
	connected = (CDCInterfaceInfo->State.ControlLineStates.HostToDevice & CDC_CONTROL_LINE_OUT_DTR);
}
