/**
  Copyright (C) 2017  Haroldo Gamal

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <avr/io.h>
#include <stdint.h>

#include "dsp.h"
//#include "global.h"
                                                      
uint8_t gu8DSP_TransceiverActualState = DSP_STATE_IDLE; /* This variable is used to know the actual state 
                                                         of the transciever */

uint8_t gu8DSP_CommunicationActualState = 0;                     /* Used to know the Main actual state of the DSP state machine */

uint32_t gu32Address;                                   /* This variable is used by the Transciever state machine 
                                                         to know the address where the data will be stored */

uint32_t gu32Data;                                      /* This is the data that will be sent to the DSP when requested */

uint8_t  gu8Knob_buf[6];

/* This is the compiled code for the DSP that expands the DSP program memory to 1.25k */
const uint8_t gu8CodeToExpandDSP_RAM[] = {0x0A ,0xFA, 0x67,     
                                        0x0A, 0xF0, 0x80, 
                                        0xFF, 0x00, 0x00};


#define DSP_MEMORY_EXPANSION_CODE           (uint8_t*)&gu8CodeToExpandDSP_RAM         /* Location where the code is stored */
#define DSP_MEMROY_EXPANSION_SIZE           (uint32_t) 3                              /* Size in words of the compiled code */
#define DSP_MEMROY_EXPANSION_START_ADDR     0                                       /* RAM address where the code will be located */

/**
 * \brief   This function will bootload the DSP with the code stored in location 'FLASH_START_ADDRESS_DSP_CODE'
            This function is divided in 3 steps:
              0. Reset DSP 
              1. Send code to expand the program RAM memory in DSP (This step is optional)
              2. Send the number of words that will be transfered
              3. Send the starting address of RAM where the code in the DSP will be stored
              4. Send next word (DSP code) until finish
            
            The above steps are executed one by one every time the function is called, so it doesn't
            block the MCU resources.
            
 * \author  Samuel Quiroz \n B05113
 * \return  none
 * \warning none
*/  

void DSP_bootload(void)
{
    /* The first step is to configure the DSP to expand the program RAM size to 1.25k. 
       This is performed by downloading the code stored in DSP_MEMORY_EXPANSION_CODE.
       
       To expand memory size the MCU sends the following data to the DSP:
          Size = 3 words
          Load in RAM address = 0
          Words to be loaded = 0x0AFA67, 0x0AF080, 0xFF0000

       
       After that the bootload starts as described below:
    
       At this moment the DSP code is located in the FLASH_START_ADDRESS_DSP_CODE in Flash.
       The first word is the code size (in words) and the second word is the starting RAM addres for
       the DSP to bootlad.

       As reflected in section 4.4 of the 56364 User Manual, the factory programmed
       bootstrap program expects the following:
          1. Three bytes defining the number of 24-bit program words to be loaded
          2. Three bytes defining the 24-bit start address to which the program loads
              in the 56364 program memory
          3. The program, three bytes for each 24 bit program word.
     */

                                        /* These variables are defined static so they don't loose their values
                                           after this function exits */
    static uint8_t  *ptrDSPCode;          /* This will be the pointer to the bytes that will be sent to the DSP */
    static uint32_t u32CodeSize;          /* This variable will store the number of words to be sent to the DSP */
    static uint8_t u8BootLoadStep = 0;    /* This variable is used to know the actual state of this function    */
    static uint8_t u8BootLoadSubStep = 0; /* This variable is used to know the sub step to expand DSP memory  */
    
    if (u8BootLoadStep == 0)
    {
        /* The step 0 is to reset the DSP */               
        uint16_t u16x = 0;
        
        MASTER_RST(0);             /* set reset line down for min 500 ns min */
        while(u16x++<6000);
        MASTER_RST(1);
        
      #ifdef EXPAND_DSP_MEMORY      /* Delete EXPAND_DSP_MEMORY from DSP.h to avoid expanding the program memory of DSP */        
        u8BootLoadStep++;           
      #else
        u8BootLoadStep = 2;         /* If EXPAND_DSP_MEMORY not defined, skip the step of expanding the program memory of DSP */
      #endif
    }

    else if (u8BootLoadStep == 1)
    {
        /* Step 1 is sending the instructions to expand the DSP RAM to 1.5k. This step is optional.
        
           This step is sub-divided in 3 steps (u8BootLoadSubStep):
              0. Send the size of the code (DSP_MEMROY_EXPANSION_SIZE = 3 words)
              1. Send the address where the code will be stored (DSP_MEMROY_EXPANSION_START_ADDR = 0)
              2. Send the words of code
        */                 
        
        if (u8BootLoadSubStep == 0)
        {

            if (DSP_HREQ_READY)
            {                                
                SET_SS();                                         /* Set SS line to start SPI transfer  */
                
                ptrDSPCode = (uint8_t*) DSP_MEMORY_EXPANSION_CODE;  /* Point to where the code is stored in Flash */
                
                u32CodeSize = DSP_MEMROY_EXPANSION_SIZE;
                SPI_sendWord(u32CodeSize);                         /* Send the code size to the DSP      */
                
                u8BootLoadSubStep++;
            }
          
        }

        else if (u8BootLoadSubStep == 1)
        {
            if (DSP_HREQ_READY)
            {
                SPI_sendWord(DSP_MEMROY_EXPANSION_START_ADDR);     /* Send the starting address where code will be stored */              
                u8BootLoadSubStep++;
            }
        }

        else if (u8BootLoadSubStep == 2)
        {
            if (u32CodeSize)   /* if there are words to send  */
            {
                if (DSP_HREQ_READY)
                {            
                    SPI_sendByte(*ptrDSPCode++);    /* Send the word  */
                    SPI_sendByte(*ptrDSPCode++); 
                    SPI_sendByte(*ptrDSPCode++); 
                    u32CodeSize--;
                }                                    /* Decrement counter  */
            }
            else
            {
                /* If there are no words to send  */
                
                u8BootLoadStep++;                     /* Go to next step  */
                u8BootLoadSubStep = 0;                /* Restore Sub-step */
                CLEAR_SS();                           /* Clear SS line to finish SPI transfer   */
            }            
        }
    }

    else if (u8BootLoadStep == 2)
    {
        /* The step 2 is to send the Code Size to DSP */       
                                       
        ptrDSPCode = (uint8_t*) FLASH_START_ADDRESS_DSP_CODE;        /* Get Code Size */
 
        u32CodeSize =  ((uint32_t)(*ptrDSPCode++)) << 16;
        u32CodeSize += ((uint32_t)(*ptrDSPCode++)) << 8;
        u32CodeSize += ((uint32_t)(*ptrDSPCode++));
        
        if (DSP_HREQ_READY)
        {                                
            SET_SS();                   /* Set SS line to start SPI transfer  */
            SPI_sendWord(u32CodeSize);   /* Send the code size to the DSP      */
            u8BootLoadStep++;           /* change to next step so when this function is executed again it will go to step 1 */
        }
         
    }    
    else if (u8BootLoadStep == 3)
    {
        /* The next step is to send the starting address of DSP RAM where the DSP code will be stored  */      
        
        uint32_t u32DSPCodeStartAddr;  
            
        /* Get DSP Code Start Address  */     
        u32DSPCodeStartAddr =  ((uint32_t)(*ptrDSPCode++)) << 16;
        u32DSPCodeStartAddr += ((uint32_t)(*ptrDSPCode++)) << 8;
        u32DSPCodeStartAddr += ((uint32_t)(*ptrDSPCode++));
        
        if (DSP_HREQ_READY)
        {
            SPI_sendWord(u32DSPCodeStartAddr);   /* Transfer starting address */
            u8BootLoadStep++;                   /* Go to next step */
        }
    }
    else if (u8BootLoadStep == 4)/* Start sending all code to DSP */
    {        
        /* This step will send all the DSP code. It will send one word by each execution, 
           so it doesn't block the main application */
        
        if (u32CodeSize)   /* if there are words to send  */
        {
            if (DSP_HREQ_READY)
            {            
                SPI_sendByte(*ptrDSPCode++);    /* Send the word  */
                SPI_sendByte(*ptrDSPCode++); 
                SPI_sendByte(*ptrDSPCode++); 
                u32CodeSize--;
            }                                    /* Decrement counter  */
        }
        else
        {
            /* If there are no words to send  */
            
            u8BootLoadStep = 0;                   /* Restore Step to 0  */
            CLEAR_SS();                           /* Clear SS line to finish SPI transfer   */
            gu8DSP_Flags.SendDataToDSP = 1;       /* Set the flag to start sending data to DSP   */
            RTC_updateDSPTimer = DSP_STARTUP_TIME;  /* Set the RTC timer to 50 ms. 
                                                     The MCU will not send data to the DSP until 
                                                     this time has elapsed. This is to give enough time to the 
                                                     DSP to initialize   */
        }
    }
        
}









/**
 * \brief   This is a pointer to functions. It handles the communication with the DSP through SPI.            
            As described in the TCDDK documentation, the SPI communication is defined as:
            
            1. Three bytes defining the command type (Read or Write)                
            2. Three bytes defining the address where the data will be stored, or the address that we want to read
            3. Three bytes defining the data in the case of a write command, or a dummy read in the case of a read command
            4. In the case of a read command, the data will be returned where this 3 bytes are read

            Note: All of the Command types and variables address in the DSP are defined in PublicVariables.h

            This pointer to function will jump to one of the following functions depending
            on the value of gu8DSP_TransceiverActualState:

                DSP_stateIdle,            
                DSP_stateTx,            
                DSP_stateRx                        
            
 * \author  Samuel Quiroz \n B05113
*/

void (*const DSP_TransceiverStateMachine[])(void) = 
{
    DSP_stateIdle,
    DSP_stateTx,
    DSP_stateRx
};

static void DSP_writeData(uint32_t u32Address, uint32_t u32Data)
{
    gu32Address = u32Address;
    gu32Data = u32Data;    
    gu8DSP_TransceiverActualState = DSP_STATE_TX;
}

static void DSP_readData(uint32_t u32Address)
{
    gu32Address = u32Address;
    gu8DSP_TransceiverActualState = DSP_STATE_RX;
}

/********************************************************************/
/*          States of the transciever for the DSP                   */
/********************************************************************/
void DSP_stateIdle(void)
{  
}
static void DSP_stateTx(void)
{
    static uint8_t u8TxState = TX_STATE_CMD;    /* This variable is used to store the actual state of the 
                                                 transmission status. */
        
    switch (u8TxState)
    {
        case TX_STATE_CMD:                    /* The 1st step is to send the write command */
        {
            if (DSP_HREQ_READY)
            {                                                
                SPI_sendWord(CMD_WRITE_TO_DSP);
                u8TxState = TX_STATE_ADDR;
            }            
            break;
        }

        case TX_STATE_ADDR:                   /* The 2nd state is to send the address  */
        {
            if (DSP_HREQ_READY)
            {
                SPI_sendWord(gu32Address);
                u8TxState = TX_STATE_DATA;
            }
            break;
        }

        case TX_STATE_DATA:                   /* The 3rd step is to send the data   */
        {
            if (DSP_HREQ_READY)
            {
                SPI_sendWord(gu32Data);
                u8TxState = TX_STATE_CMD;     /* change my state to send the command (next time a read is performed) */
                gu8DSP_TransceiverActualState = DSP_STATE_IDLE;    /* return to idle state         */
                gu8DSP_Flags.DataSent = 1;    /* Set a flag to indicate that data has been sent    */
            }
            break;
        }
    }  
}

static void DSP_stateRx(void)
{
    static uint8_t u8RxState = RX_STATE_CMD;      /* The 1st step is to send the read command  */
    
    switch (u8RxState)
    {
        case RX_STATE_CMD:
        {
            if (DSP_HREQ_READY)
            {
                SPI_sendWord(CMD_READ_FROM_DSP);
                u8RxState = RX_STATE_ADDR;
            }            
            break;
        }

        case RX_STATE_ADDR:                     /* The 2nd state is to send the address  */
        {
            if (DSP_HREQ_READY)
            {
                SPI_sendWord(gu32Address);
                u8RxState = RX_STATE_DATA0;
            }
            break;
        }

        case RX_STATE_DATA0:                    /* The 3rd step is to perform a dummy read  */
        {
            if (DSP_HREQ_READY)
            {
                (void)SPI_receiveWord();         /* dummy read  */              
                u8RxState = RX_STATE_DATA1;
            }
            break;
        }
        
        case RX_STATE_DATA1:                     /* The 4th step is to perform the read operation */
        {
            if (DSP_HREQ_READY)
            {
                gu32Data = SPI_receiveWord();
                u8RxState = TX_STATE_CMD;        /* change my state to send the command (next time a read is performed) */
                gu8DSP_TransceiverActualState = DSP_STATE_IDLE;     /* return to idle state                             */
                gu8DSP_Flags.DataSent = 1;       /* set flag to indicate that the read was performed                    */
            }
            break;
        }
    }   
}
void (*const DSP_communicationStateMachine[])(void) = 
{
    DSP_Knob1,
    DSP_Knob2,
    DSP_Knob3,
    DSP_Knob4,
    DSP_Knob5,
    DSP_Knob6,
    DSP_Switch1,
    DSP_Switch2,
    DSP_FootSwitchTap,
    DSP_FootSwitch,
    DSP_LEDRed,
    DSP_LEDGreen,
    DSP_DebugVar1ToDSP,
    DSP_DebugVar2ToDSP,
    DSP_DebugVar3ToDSP,
    DSP_DebugVar4ToDSP,
    DSP_DebugVar1FromDSP,
    DSP_DebugVar2FromDSP,
    DSP_DebugVar3FromDSP,
    DSP_DebugVar4FromDSP    
};
  
void DSP_stateMachineDSP(void)
{
    DSP_communicationStateMachine[gu8DSP_CommunicationActualState]();            
    DSP_TransceiverStateMachine[gu8DSP_TransceiverActualState]();    
}

/***************************************************************************************************************************
 *
 *    Each of the functions below will perform a read or a write to the DSP.
 *    It will do the following:
 *        1. When the Transciever is Idle it will start a read or write transaction
 *        2. When the Transciever has finished the transaction:
 *            - If it was a read operation, it will store the received data in the correct variable (See PublicVariables.c)
 *            - Move to the next state. If the last state has been reached, it will return to the first one. 
 *
 *    NOTE 1: Knobs values are read and stored as bytes but they are sent to the DSP shifted 15 positions 
 *            to the left in order to have a range from 0x000000 to 0x7F80000, instead of 0x00 to 0xFF
 *            This is because the DSP makes calculations using 24bit numbers
 *
 *    NOTE 2: Debug variables are stored as t_UINT24 (like an array of 3 bytes)
 *            This is why they are being read or written as shifted bytes
 * 
 ****************************************************************************************************************************/

static void DSP_Knob1(void)
{ 
  uint32_t Knob_avg;                                      /* Knob Jitter Filtering Start */
  uint32_t Knob_val;
    
    Knob_avg = (gu8Knob_buf[0] + KNOB_1) >> 1;          //Average current and previous knob values
    Knob_val = Knob_avg << 15;                          //extend knob value to a 24 bit value
    Knob_val += Knob_avg << 7;                          //add multiples of knob value so that KNOB_MAX = 0x7FFFFF
    Knob_val += Knob_avg >> 1;
    gu8Knob_buf[0] = KNOB_1;                            /* Knob Jitter Filtering End */
    
    if (gu8DSP_TransceiverActualState == DSP_STATE_IDLE)
    {
        if (gu8DSP_Flags.DataSent == 0)
        {
            SET_SS();                                    
            DSP_writeData(KNOB_1_ADDR_DSP,(Knob_val));
        }
        else /* if(gu8DSP_Flags.DataSent == 1)  */
        {                
            gu8DSP_Flags.DataSent = 0;
            CLEAR_SS();
            gu8DSP_CommunicationActualState++;
        }                                
    }
}

static void DSP_Knob2(void)
{
  uint32_t Knob_avg;                                      /* Knob Jitter Filtering Start */
  uint32_t Knob_val;
    
    Knob_avg = (gu8Knob_buf[1] + KNOB_2) >> 1;          //Average current and previous knob values
    Knob_val = Knob_avg << 15;                          //extend knob value to a 24 bit value
    Knob_val += Knob_avg << 7;                          //add multiples of knob value so that KNOB_MAX = 0x7FFFFF
    Knob_val += Knob_avg >> 1;
    gu8Knob_buf[1] = KNOB_2;                            /* Knob Jitter Filtering End */
    
    if (gu8DSP_TransceiverActualState == DSP_STATE_IDLE)
    {
        if (gu8DSP_Flags.DataSent == 0)
        {
            SET_SS();                                    
            DSP_writeData(KNOB_2_ADDR_DSP,Knob_val);  
        }
        else /* if(gu8DSP_Flags.DataSent == 1)   */
        {                
            gu8DSP_Flags.DataSent = 0;
            CLEAR_SS();
            gu8DSP_CommunicationActualState++;
        }                                
    }  
}

static void DSP_Knob3(void)
{
  uint32_t Knob_avg;                                      /* Knob Jitter Filtering Start */
  uint32_t Knob_val;
    
    Knob_avg = (gu8Knob_buf[2] + KNOB_3) >> 1;          //Average current and previous knob values
    Knob_val = Knob_avg << 15;                          //extend knob value to a 24 bit value
    Knob_val += Knob_avg << 7;                          //add multiples of knob value so that KNOB_MAX = 0x7FFFFF
    Knob_val += Knob_avg >> 1;
    gu8Knob_buf[2] = KNOB_3;                            /* Knob Jitter Filtering End */

    if (gu8DSP_TransceiverActualState == DSP_STATE_IDLE)
    {
        if (gu8DSP_Flags.DataSent == 0)
        {
            SET_SS();                                    
            DSP_writeData(KNOB_3_ADDR_DSP,Knob_val);  
        }
        else /* if(gu8DSP_Flags.DataSent == 1)   */
        {                
            gu8DSP_Flags.DataSent = 0;
            CLEAR_SS();
            gu8DSP_CommunicationActualState++;
        }                                
    }  
}

static void DSP_Knob4(void)
{
  uint32_t Knob_avg;                                      /* Knob Jitter Filtering Start */
  uint32_t Knob_val;
    
    Knob_avg = (gu8Knob_buf[3] + KNOB_4) >> 1;          //Average current and previous knob values
    Knob_val = Knob_avg << 15;                          //extend knob value to a 24 bit value
    Knob_val += Knob_avg << 7;                          //add multiples of knob value so that KNOB_MAX = 0x7FFFFF
    Knob_val += Knob_avg >> 1;
    gu8Knob_buf[3] = KNOB_4;                            /* Knob Jitter Filtering End */

    if (gu8DSP_TransceiverActualState == DSP_STATE_IDLE)
    {
        if (gu8DSP_Flags.DataSent == 0)
        {
            SET_SS();                                    
            DSP_writeData(KNOB_4_ADDR_DSP,Knob_val);  
        }
        else /* if(gu8DSP_Flags.DataSent == 1) */
        {                
            gu8DSP_Flags.DataSent = 0;
            CLEAR_SS();
            gu8DSP_CommunicationActualState++;
        }                                
    }  
}

static void DSP_Knob5(void)
{
  uint32_t Knob_avg;                                      /* Knob Jitter Filtering Start */
  uint32_t Knob_val;
    
    Knob_avg = (gu8Knob_buf[4] + KNOB_5) >> 1;          //Average current and previous knob values
    Knob_val = Knob_avg << 15;                          //extend knob value to a 24 bit value
    Knob_val += Knob_avg << 7;                          //add multiples of knob value so that KNOB_MAX = 0x7FFFFF
    Knob_val += Knob_avg >> 1;
    gu8Knob_buf[4] = KNOB_5;                            /* Knob Jitter Filtering End */

    if (gu8DSP_TransceiverActualState == DSP_STATE_IDLE)
    {
        if (gu8DSP_Flags.DataSent == 0)
        {
            SET_SS();                                    
            DSP_writeData(KNOB_5_ADDR_DSP,Knob_val);  
        }
        else /* if(gu8DSP_Flags.DataSent == 1)  */
        {                
            gu8DSP_Flags.DataSent = 0;
            CLEAR_SS();
            gu8DSP_CommunicationActualState++;
        }                                
    }  
}


static void DSP_Knob6(void)	
{
  uint32_t Knob_avg;                                      /* Knob Jitter Filtering Start */
  uint32_t Knob_val;
    
    Knob_avg = (gu8Knob_buf[5] + KNOB_6) >> 1;          //Average current and previous knob values
    Knob_val = Knob_avg << 15;                          //extend knob value to a 24 bit value
    Knob_val += Knob_avg << 7;                          //add multiples of knob value so that KNOB_MAX = 0x7FFFFF
    Knob_val += Knob_avg >> 1;
    gu8Knob_buf[5] = KNOB_6;                            /* Knob Jitter Filtering End */

    if (gu8DSP_TransceiverActualState == DSP_STATE_IDLE)
    {
        if (gu8DSP_Flags.DataSent == 0)
        {
            SET_SS();                                    
            DSP_writeData(KNOB_6_ADDR_DSP,Knob_val);  
        }
        else /* if(gu8DSP_Flags.DataSent == 1)  */
        {                
            gu8DSP_Flags.DataSent = 0;
            CLEAR_SS();
            gu8DSP_CommunicationActualState++;
        }                                
    }  
}

static void DSP_Switch1(void)
{
    if (gu8DSP_TransceiverActualState == DSP_STATE_IDLE)
    {
        if (gu8DSP_Flags.DataSent == 0)
        {
            SET_SS();                                    
            DSP_writeData(SWITCH_1_ADDR_DSP,SWITCH_1);  
        }
        else /* if(gu8DSP_Flags.DataSent == 1)   */
        {                
            gu8DSP_Flags.DataSent = 0;
            CLEAR_SS();
            gu8DSP_CommunicationActualState++;
        }                                
    }  
}

static void DSP_Switch2(void)
{
    if (gu8DSP_TransceiverActualState == DSP_STATE_IDLE)
    {
        if (gu8DSP_Flags.DataSent == 0)
        {
            SET_SS();                                    
            DSP_writeData(SWITCH_2_ADDR_DSP,SWITCH_2);  
        }
        else /* if(gu8DSP_Flags.DataSent == 1)    */
        {                
            gu8DSP_Flags.DataSent = 0;
            CLEAR_SS();
            gu8DSP_CommunicationActualState++;
        }                                
    }  
}

static void DSP_FootSwitchTap(void)
{
    if (gu8DSP_TransceiverActualState == DSP_STATE_IDLE)
    {
        if (gu8DSP_Flags.DataSent == 0)
        {
            SET_SS();                                    
            DSP_writeData(FOOT_SWITCH_TAP_ADDR_DSP,FOOT_SWITCH_TAP);  
        }
        else /* if(gu8DSP_Flags.DataSent == 1)   */
        {                
            gu8DSP_Flags.DataSent = 0;
            CLEAR_SS();
            gu8DSP_CommunicationActualState++;
        }                                
    }  
}


static void DSP_FootSwitch(void)
{
    if (gu8DSP_TransceiverActualState == DSP_STATE_IDLE)
    {
        if (gu8DSP_Flags.DataSent == 0)
        {
            SET_SS();                                    
            DSP_writeData(FOOT_SWITCH_ADDR_DSP,FOOT_SWITCH);  
        }
        else /* if(gu8DSP_Flags.DataSent == 1)   */
        {                
            gu8DSP_Flags.DataSent = 0;
            CLEAR_SS();
            gu8DSP_CommunicationActualState++;
        }                                
    }  
}


static void DSP_LEDRed(void)
{
    if (gu8DSP_TransceiverActualState == DSP_STATE_IDLE)
    {
        if (gu8DSP_Flags.DataSent == 0)
        {
            SET_SS();                                    
            DSP_readData(RED_LED_ADDR_DSP);  
        }
        else /* if(gu8DSP_Flags.DataSent == 1)    */
        {                
            RED_LED = (uint8_t)gu32Data;
            gu8DSP_Flags.DataSent = 0;
            CLEAR_SS();
            gu8DSP_CommunicationActualState++;
        }                                
    }   
}

static void DSP_LEDGreen(void)
{
    if (gu8DSP_TransceiverActualState == DSP_STATE_IDLE)
    {
        if (gu8DSP_Flags.DataSent == 0)
        {
            SET_SS();                                    
            DSP_readData(GREEN_LED_ADDR_DSP);  
        }
        else /* if(gu8DSP_Flags.DataSent == 1) */
        {                
            GREEN_LED = (uint8_t)gu32Data;
            gu8DSP_Flags.DataSent = 0;
            CLEAR_SS();
            gu8DSP_CommunicationActualState++;
        }                                
    }  
}

static void DSP_DebugVar1ToDSP(void)
{
    if (gu8DSP_TransceiverActualState == DSP_STATE_IDLE)
    {
        if (gu8DSP_Flags.DataSent == 0)
        {
            gu32Data =  ((uint32_t)DEBUG_VAR_TO_DSP_1.Byte2)<<16;            
            gu32Data += ((uint32_t)DEBUG_VAR_TO_DSP_1.Byte1)<<8;
            gu32Data += ((uint32_t)DEBUG_VAR_TO_DSP_1.Byte0);

            SET_SS();                                                
            DSP_writeData(DEBUG_VAR_TO_DSP_1_ADDR_DSP, gu32Data);
        }
        else /* if(gu8DSP_Flags.DataSent == 1)  */
        {                
            gu8DSP_Flags.DataSent = 0;
            CLEAR_SS();
            gu8DSP_CommunicationActualState++;
        }                                
    }  
}

static void DSP_DebugVar2ToDSP(void)
{
    if (gu8DSP_TransceiverActualState == DSP_STATE_IDLE)
    {
        if (gu8DSP_Flags.DataSent == 0)
        {
            SET_SS();                                    
            gu32Data =  ((uint32_t)DEBUG_VAR_TO_DSP_2.Byte2)<<16;            
            gu32Data += ((uint32_t)DEBUG_VAR_TO_DSP_2.Byte1)<<8;
            gu32Data += ((uint32_t)DEBUG_VAR_TO_DSP_2.Byte0);
            
            DSP_writeData(DEBUG_VAR_TO_DSP_2_ADDR_DSP, gu32Data);
        }
        else /* if(gu8DSP_Flags.DataSent == 1)  */
        {                
            gu8DSP_Flags.DataSent = 0;
            CLEAR_SS();
            gu8DSP_CommunicationActualState++;
        }                                
    }  
}

static void DSP_DebugVar3ToDSP(void)
{
    if (gu8DSP_TransceiverActualState == DSP_STATE_IDLE)
    {
        if (gu8DSP_Flags.DataSent == 0)
        {
            SET_SS();                                    
            gu32Data =  ((uint32_t)DEBUG_VAR_TO_DSP_3.Byte2)<<16;            
            gu32Data += ((uint32_t)DEBUG_VAR_TO_DSP_3.Byte1)<<8;
            gu32Data += ((uint32_t)DEBUG_VAR_TO_DSP_3.Byte0);
            
            DSP_writeData(DEBUG_VAR_TO_DSP_3_ADDR_DSP, gu32Data);
        }
        else /* if(gu8DSP_Flags.DataSent == 1)  */
        {                
            gu8DSP_Flags.DataSent = 0;
            CLEAR_SS();
            gu8DSP_CommunicationActualState++;
        }                                
    } 
}

static void DSP_DebugVar4ToDSP(void)
{
    if (gu8DSP_TransceiverActualState == DSP_STATE_IDLE)
    {
        if (gu8DSP_Flags.DataSent == 0)
        {
            SET_SS();                                    
            gu32Data =  ((uint32_t)DEBUG_VAR_TO_DSP_4.Byte2)<<16;            
            gu32Data += ((uint32_t)DEBUG_VAR_TO_DSP_4.Byte1)<<8;
            gu32Data += ((uint32_t)DEBUG_VAR_TO_DSP_4.Byte0);
            
            DSP_writeData(DEBUG_VAR_TO_DSP_4_ADDR_DSP, gu32Data);
        }
        else /* if(gu8DSP_Flags.DataSent == 1) */
        {                
            gu8DSP_Flags.DataSent = 0;
            CLEAR_SS();
            gu8DSP_CommunicationActualState++;
        }                                
    }  
}

static void DSP_DebugVar1FromDSP(void)
{
    if (gu8DSP_TransceiverActualState == DSP_STATE_IDLE)
    {
        if (gu8DSP_Flags.DataSent == 0)
        {
            SET_SS();                                                
            DSP_readData(DEBUG_VAR_FROM_DSP_1_ADDR_DSP);
        }
        else /* if(gu8DSP_Flags.DataSent == 1) */
        {                
            DEBUG_VAR_FROM_DSP_1.Byte2 = (uint8_t) (gu32Data>>16);
            DEBUG_VAR_FROM_DSP_1.Byte1 = (uint8_t) (gu32Data>>8);
            DEBUG_VAR_FROM_DSP_1.Byte0 = (uint8_t) (gu32Data);
                        
            gu8DSP_Flags.DataSent = 0;
            CLEAR_SS();
            gu8DSP_CommunicationActualState++;
        }                                
    }  
}

static void DSP_DebugVar2FromDSP(void)
{
    if (gu8DSP_TransceiverActualState == DSP_STATE_IDLE)
    {
        if (gu8DSP_Flags.DataSent == 0)
        {
            SET_SS();                                                
            DSP_readData(DEBUG_VAR_FROM_DSP_2_ADDR_DSP);
        }
        else /* if(gu8DSP_Flags.DataSent == 1)  */
        {                
            DEBUG_VAR_FROM_DSP_2.Byte2 = (uint8_t) (gu32Data>>16);
            DEBUG_VAR_FROM_DSP_2.Byte1 = (uint8_t) (gu32Data>>8);
            DEBUG_VAR_FROM_DSP_2.Byte0 = (uint8_t) (gu32Data);
                        
            gu8DSP_Flags.DataSent = 0;
            CLEAR_SS();
            gu8DSP_CommunicationActualState++;
        }                                
    }  
}

static void DSP_DebugVar3FromDSP(void)
{
    if (gu8DSP_TransceiverActualState == DSP_STATE_IDLE)
    {
        if (gu8DSP_Flags.DataSent == 0)
        {
            SET_SS();                                                
            DSP_readData(DEBUG_VAR_FROM_DSP_3_ADDR_DSP);
        }                                         
        else /* if(gu8DSP_Flags.DataSent == 1)  */
        {                
            DEBUG_VAR_FROM_DSP_3.Byte2 = (uint8_t) (gu32Data>>16);
            DEBUG_VAR_FROM_DSP_3.Byte1 = (uint8_t) (gu32Data>>8);
            DEBUG_VAR_FROM_DSP_3.Byte0 = (uint8_t) (gu32Data);
                        
            gu8DSP_Flags.DataSent = 0;
            CLEAR_SS();
            gu8DSP_CommunicationActualState++;
        }                                
    }  
}

static void DSP_DebugVar4FromDSP(void)
{
    if (gu8DSP_TransceiverActualState == DSP_STATE_IDLE)
    {
        if (gu8DSP_Flags.DataSent == 0)
        {
            SET_SS();                                                
            DSP_readData(DEBUG_VAR_FROM_DSP_4_ADDR_DSP);
        }
        else /* if(gu8DSP_Flags.DataSent == 1)  */
        {                
            DEBUG_VAR_FROM_DSP_4.Byte2 = (uint8_t) (gu32Data>>16);
            DEBUG_VAR_FROM_DSP_4.Byte1 = (uint8_t) (gu32Data>>8);
            DEBUG_VAR_FROM_DSP_4.Byte0 = (uint8_t) (gu32Data);
                        
            gu8DSP_Flags.DataSent = 0;
            CLEAR_SS();
            gu8DSP_CommunicationActualState=0;
        }                                
    }  
}
