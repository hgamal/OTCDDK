#ifndef _DSP_H
#define _DSP_H

//#include "gpio.h"
//#include "adc.h"
#include "spi.h"
//#include "rtc.h"
//#include "switches.h"

#define FLASH_START_ADDRESS_DSP_CODE    0xC000            //Starting address where the DSP code will be stored in the MCU
#define DSP_HREQ_READY                  ((PINB & _BV(PINB1)) == 0)     //Used to know if the DSP is ready for a SPI transfer
#define SET_SS()                        PINB &= ~_BV(PINB2)       //Used to start a SPI transfer
#define CLEAR_SS()                      PINB |= _BV(PINB2)       //Used to stop a SPI transfer

#define DSP_STARTUP_TIME                50                 /* Set the RTC timer to 50 ms. 
                                                     The MCU will not send data to the DSP until 
                                                     this time has elapsed. This is to give enough time to the 
                                                     DSP to initialize   */

#define EXPAND_DSP_MEMORY              /* Delete this line to avoid expanding the program memory of DSP */

/* Declarations */

typedef enum
{
    DSP_STATE_IDLE,
    DSP_STATE_TX,
    DSP_STATE_RX,
}_States;


typedef enum
{
    TX_STATE_CMD,
    TX_STATE_ADDR,
    TX_STATE_DATA
} _TxStates;

typedef enum
{
    RX_STATE_CMD,
    RX_STATE_ADDR,
    RX_STATE_DATA0,
    RX_STATE_DATA1
} _RxStates;

void StateMachineDSP(void);
void DSP_Bootload(void);

/* Private functions: */
static void DSP_writeData(uint32_t u32Address, uint32_t u32Data);
static void DSP_readData(uint32_t u32Address);
static void DSP_stateIdle(void);
static void DSP_stateTx(void);
static void DSP_stateRx(void);

static void DSP_Knob1(void);
static void DSP_Knob2(void);
static void DSP_Knob3(void);
static void DSP_Knob4(void);
static void DSP_Knob5(void);
static void DSP_Knob6(void);
static void DSP_Switch1(void);
static void DSP_Switch2(void);
static void DSP_FootSwitchTap(void);
static void DSP_FootSwitch(void);
static void DSP_LEDRed(void);
static void DSP_LEDGreen(void);
static void DSP_DebugVar1ToDSP(void);
static void DSP_DebugVar2ToDSP(void);
static void DSP_DebugVar3ToDSP(void);
static void DSP_DebugVar4ToDSP(void);
static void DSP_DebugVar1FromDSP(void);
static void DSP_DebugVar2FromDSP(void);
static void DSP_DebugVar3FromDSP(void);
static void DSP_DebugVar4FromDSP(void);

void (*const DSP_communicationStateMachine[])(void);

#endif /* _DSP_H */
