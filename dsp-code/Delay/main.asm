                page    132,60
                include "..\Ioequ.inc"
                include "..\vectors4.inc"                
                list

;---------------------------------------------------------------------------------------------------
;   Delay
;
;	Simple Digital Delay
;
;   Copyright 2018 Haroldo Gamal <haroldo@gamal.com.br>
;
;       Knob1 - mix
;       Knob2 - delay
;       Knob3 - Feedback
;
;---------------------------------------------------------------------------------------------------

        org     x:$000
;...Knob registers expect a value of 0x000000 thru 0x7FFFFF from the microcontroller
Knob_1                  ds  1       ; 00h
Knob_2                  ds  1       ; 01h
Knob_3                  ds  1       ; 02h
Knob_4                  ds  1       ; 03h
Knob_5                  ds  1       ; 04h
Knob_6                  ds  1       ; 05h

;...Switch registers expect a value of 0,1, or 2 from the microcontroller
Switch_1                ds  1       ; 06h
Switch_2                ds  1       ; 07h

;...FootSwitch registers expect a value of 0x000000 or 0x000001 from the microcontroller
FootSwitch_TopLayer     ds  1       ; 08h
FootSwitch_BottomLayer  ds  1       ; 09h

;...LED registers are read by the micro and should be set to 0x000000 or 0x000001  (off and on)
LED_Red                 ds  1       ; 0ah
LED_Green               ds  1       ; 0bh

;...RX and TX registers
LeftRx                  ds  1       ; 0ch   Left received sample
RightRx                 ds  1       ; 0dh   Right received sample
LeftTx                  ds  1       ; 0eh   Left sample to transmit
RightTx                 ds  1       ; 0fh   Right sample to transmit
SHI_StateMachine        ds  1       ; 10h   current state of the serial host interface
HostCommand             ds  1       ; 11h   current SHI command

;...Debug registers
Debug_Write_to_DSP_1    ds  1       ; 12h   recieves data from the ToneCoreGUI application
Debug_Write_to_DSP_2    ds  1       ; 13h   recieves data from the ToneCoreGUI application
Debug_Write_to_DSP_3    ds  1       ; 14h   recieves data from the ToneCoreGUI application
Debug_Write_to_DSP_4    ds  1       ; 15h   recieves data from the ToneCoreGUI application
Debug_Read_from_DSP_1   ds  1       ; 16h   send data to the ToneCoreGUI application
Debug_Read_from_DSP_2   ds  1       ; 17h   send data to the ToneCoreGUI application
Debug_Read_from_DSP_3   ds  1       ; 18h   send data to the ToneCoreGUI application
Debug_Read_from_DSP_4   ds  1       ; 19h   send data to the ToneCoreGUI application


;...The following registers are used to store filter coefficients
FootLatch               ds  1       ; 20h
FootLatchMem            ds  1       ; 21h

        org     y:$000              
;...copy of current input and output samples
LeftInput               ds  1       ; 0x00
LeftOutput              ds  1       ; 0x01
RightInput              ds  1       ; 0x02
RightOutput             ds  1       ; 0x03
Chan0IO                 ds  1       ; 0x05
Chan1IO                 ds  1       ; 0x06
DelayPotValue           ds  1       ; 0x07
DelaySize               ds  1       ; 0x08
efil                    ds  1       ; 0x09
pacc                    ds  1       ; 0x0a

BufferSize      equ     120000

	org		y:$200000

DelayBuffer	dsm	(BufferSize*3)

;**************************************************************************

        org     p:$4E
START
        ori     #$03,mr                 ; mask interrupts
        movep   #$2D0063,X:M_PCTL       ; Set PLL Control Register
                                        ; Fosc = 100 MHz = (100)(3 MHz)/3
        
        bset    #14,omr                 ; allow address attributes line to function independently
        
        movep   #>$000040,x:M_SAICR    ; 4x4 Synchronous mode (use TX frame and bit clks)

        movep   #$FCC304,x:M_TCCR                           
                ;THCKD -1- HCKT is an output                          (bit  23)
                ;TFSD  -1- FST is output                              (bit  22)
                ;TCKD  -1- internal clock source drives SCKT          (bit  21)
                ;THCKP -1- Transmitter High Freq Clock Polarity       (bit  20)
                ;TFSP  -1- negative FST polarity                      (bit  19)
                ;TCKP  -1- data & FST clocked out on falling edge     (bit  18)
                ;TFP   -3- TFP3..0 = Divide by 4                      (bits 17:14)
                ;TDC   -1- 2 words per frame                          (bits 13:9)
                ;TPSR  -1- Bypass Fixed /8 Prescaler                  (bit  8)
                ;TPM   -4- TPM7-0 = Divide by 5                       (bits 7:0)

         movep  #$FCC304,x:M_RCCR                          
                ;RHCKD -1- HCKR is an output  Flag 2                  (bit  23)
                ;RFSD  -1- FSR is output      Flag 1                  (bit  22)
                ;RCKD  -1- SCKR is output     Flag 0                  (bit  21)
                ;RHCKP -1- Pos. High Freq Clock Polarity              (bit  20)
                ;RFSP  -1- negative FSR polarity                      (bit  19)
                ;RCKP  -1- data & FSR clocked in on falling edge      (bit  18)
                ;RFP   -3- RFP3..0 = Divide by 4                      (bits 17:14)
                ;RDC   -1- 2 words per frame                          (bit  13:9)
                ;RPSR  -1- Bypass Fixed /8 Prescaler                  (bit  8)
                ;RPM   -4- RPM7-0 = Divide by 5                       (bits 7:0)

        movep   #$707d00,x:M_RCR                            
                ;RE    --- RX0, RX1, RX2, RX3 disabled                (bit3:0=0000)
                ;RSHFD -0- MSB shifted first                          (bit6=0)
                ;RWA   -0- word left-aligned                          (bit7=0)
                ;RMOD  -1- network mode                               (bit9:8=01)
                ;RSWS  1F- 32-bit slot length, 24-bit word length     (bit14:10=11111)
                ;RFSL  -0- word-length frame sync                     (bit15=0)
                ;RFSR  -0- frame sync occurs 1 clock cycle earlier    (bit16=1)
                ;          reserved                                   (bit18:17=00)
                ;RPR   ?-0- transmit normally, not personal reset      (bit19=0)
                ;          RIE, REDIE, REIE enabled                  (bit23:20=0111)
                ;RLIE  --- bit23 RLIE
                ;RIE   --- bit22 RIE
                ;REDIE --- bit21 REDIE
                ;REIE  --- bit20 REIE

        movep   #$027D80,x:M_TCR                            
                ;TE    --- Start w/ TX0-TX5 disabled                  (bit5:0=000000)
                ;TSHFD -0- MSB shifted first                          (bit6=0)
                ;TWA   -1- word left-aligned                          (bit7=0)
                ;TMOD  -1- network mode                               (bit9:8=01)
                ;TSWS  1F- 32-bit slot length, 24-bit word length     (bit14:10=11111)
                ;TFSL  -0- word length frame sync                     (bit15=0)
                ;TFSR  -0- frame sync occurs 1 clock cycle earlier    (bit16=1)
                ;PADC  -1- zero padding enabled                       (bit17=1)
                ;          reserved                                   (bit18)
                ;TRP   ?-0- transmit normally, not personal reset      (bit19=0)
                ;          TLIE, TIE, TEIE disabled                   (bit23:20=0000)
                ;TLIE  --- bit23 TLIE
                ;TIE   --- bit22 TIE
                ;TEDIE --- bit21 TEDIE
                ;TEIE  --- bit20 TEIE

        
        movep   #>$000000,x:M_PDRC      ; Clear Port C data
        movep   #>$000BF8,x:M_PCRC      ; Set appropriate Port C GPIO pins for ESAI .
        movep   #>$000C7E,x:M_PRRC      ; Set pin direction of PORT C
        ;sdo0     sdo1         sdo2         sdo3    sdo4    sdo5        hckt        fst         sckt    hckr            fsr         sckr
        ;ESAI     GPO          GPI          GPI     GPI     ESAI        ESAI        ESAI        ESAI    GPO             GPO         NC
        ;DAC Data Inhibit IRQA Stereo/Mono  A_In    B_In    ADC_Data    256FS_CLK0  FS_CLK  64FS_CLK    Gain Switching  Codec_Reset SS_Module


        movep   #>$000000,x:M_PCRB      ; Set up Port B for output
        movep   #>$00000F,x:M_PRRB      ; Set up Port B for output
        movep   #>$000008,x:M_PDRB      ; bit 0 = In_EMPH 
                                        ; bit 1 = OUT_DE_EMPH
                                        ; bit 2 = DIRECT_ON
                                        ; bit 3 = FX_ON

        movep   #>$000003,x:M_RSMA      ; Enable first 2 time slots for receive.
        movep   #>$000000,x:M_RSMB      ;
        movep   #>$000003,x:M_TSMA      ; Enable first 2 time slots for transmit.
        movep   #>$000000,x:M_TSMB      
        movep   #>$000000,x:M_TX0       ; zero out transmitter 0

        ;ENABLE ESAI
        bset    #0,x:M_RCR              ; now enable RX0
        bset    #0,x:M_TCR              ; now enable TX0

;...Setup Expansion Port A for SRAM...
        movep   #$2406B5,x:M_AAR0                
                 ; [23:12] = 0x240 Address used to assert chip select
                 ; [11:8]  = 0110  Number of address bits to compare = 6 
                 ; [7]     = 1     Bit Packing Enabled
                 ; [6]     = 0     Address Mux Disabled
                 ; [5]     = 1     Y Space Enabled
                 ; [4]     = 1     X Space Enabled
                 ; [3]     = 0     P Space Disabled
                 ; [2]     = 1     Active High for Address Line  
                 ; [1:0]   = 01    SRAM Mode
                 
        movep   #$2003B1,x:M_AAR1
                 ; [23:12] = 0x200 Address used to assert chip select
                 ; [11:8]  = 0011  Number of address bits to compare = 3 
                 ; [7]     = 1     Bit Packing Enabled
                 ; [6]     = 0     Address Mux Disabled
                 ; [5]     = 1     Y Space Enabled
                 ; [4]     = 1     X Space Enabled
                 ; [3]     = 0     P Space Disabled
                 ; [2]     = 0     Active Low for Chip Select
                 ; [1:0]   = 01    SRAM Mode


;...Bus Control Register for SRAM...
        movep   #$0124A5,x:M_BCR                ;        000 00001 001 001 00101 00101
                ;bus request hold off           (bit23=0)                   0 BRH
                ;bus lock hold off              (bit22=0)                   0 BLH
                ;bus state                      (bit21=0)                   0 BBS
                ;default area wait states       (bit20:16=00001)        00001 BDFW[0-4]
                ;area 3 wait states             (bit15:13=001)            001 BA3W[0-2]
                ;area 2 wait states             (bit12:10=001)            001 BA2W[0-2]
                ;area 1 wait states             (bit9:5=00101)          00101 BA1W[0-4]
                ;area 0 wait states             (bit4:0=00101)          00101 BA0W[0-4]


;...Set up SHI (Serial Host Interface to the MCU)...
        movep   #>$003001,x:M_HCKR      ; Turn Data/Clk Line Filter to max, wide spike tolerance (100ns glitch)
                                        ; CPHA=1, CPOL=0 : => same as reset/power-on.
        movep   #>$001189,x:M_HCSR      ;

;...Initialize registers
        move    #>$000000,x0
        move    x0,r0
        rep     #26 
        move    x0,x:(r0)+              ; clear start of x:mem

        move    #>$400000,x0			; Intialize the knob registers
        move    x0,x:Knob_1     
        move    x0,x:Knob_2     
        move    x0,x:Knob_3     
        move    x0,x:Knob_4     
        move    x0,x:Knob_5     
        move    x0,x:Knob_6     

        move    #>$000000,x0
        move    #$00ffff,m5             ; Use r5 for the MCU parameter updates.
        movep   x0,x:M_HTX              ; Assert HREQ* pin for the MCU.


;...Initialize Peripheral Interrupt Priority Register for Audio Interrupts and SHI.
        movep   #$000007,x:M_IPRP       ; ESAI int's enabled and top Priority
                                        ; SHI int's enabled and lowest Priority.

        jsr     setup                   ; Init DMA

        andi    #$FC,mr                 ;enable all interrupt levels
                                        ;clear scaling bits

        movep   #>$000002,x:M_PDRC      ; Take CODEC out of power down mode.

;------------------------------------------------------------
; Main loop
;------------------------------------------------------------
LOOP
        wait
        jmp     LOOP 

;XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
;XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
;   4x4 Interrupt Service Routines
;XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
;XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX

;-----------------------------
; Host Interrupt
;-----------------------------

shi_receive
        movep   x:M_HRX,x:HostCommand	; Get word from 8031.
        movep   #000000,x:M_HTX			; Assert HREQ* pin for 8031.
        btst    #0,x:HostCommand        ; $000001 = Write 1 DSP word to X:mem.
        bcs     DoWriteCommand          ;
        btst    #3,x:HostCommand        ; $000008 = Send word to the 8031.
        bcs     DoReadCommand           ;

DoWriteCommand
        jclr    #M_HRNE,x:M_HCSR,*      ; Wait for address.
        movep   x:M_HRX,r5              ; Store address to write to.
        movep   #>000000,x:M_HTX        ; Assert HREQ* pin for 8031.

        jclr    #M_HRNE,x:M_HCSR,*      ; Wait for data 
        movep   x:M_HRX,x:(r5)          ; Write data.
        movep   #000000,x:M_HTX         ; Assert HREQ* pin for 8031.
	rti

DoReadCommand
        jclr    #M_HRNE,x:M_HCSR,*
        movep   x:M_HRX,r5              ; Store Address to read from
        movep   #>000000,x:M_HTX
        
        jclr    #M_HRNE,x:M_HCSR,*
        movep   x:M_HRX,n5
        movep   x:(r5),x:<<M_HTX        ; Send Data at specified address to the 8031
        
        jclr    #M_HRNE,x:M_HCSR,*
        movep   x:M_HRX,n5
        movep   #>000000,x:<<M_HTX
	rti


;-----------------------------
; Receive Exception Interrupt
;-----------------------------
esai_rxe_isr                            ; ESAI RECEIVE ISR

        bclr    #7,x:M_SAISR            ; Read SAISR to clear receive
                                        ; overrun error flag
        bclr    #14,x:M_SAISR           ; Read SAISR to clear transmit
                                        ; underrun error flag
        movep   x:M_RX0,x:RightRx       ; 
        movep   x:RightTx,x:M_TX0       ;

        rti

;-----------------------------
; Receive Interrupt
;-----------------------------
esai_rx_isr
        movep   x:M_RX0,x:RightRx       
        movep   x:RightTx,x:M_TX0       
        rti

;-----------------------------
; Receive Even Slot Interrupt
;-----------------------------
esai_rxeven_isr

	; r5 and n5 are reserved by the Host Interrupt process

        movep   x:M_RX0,x:LeftRx        
        movep   x:LeftTx,x:M_TX0        

        move    y:LeftOutput,x0         ; 
        move    x0,x:LeftTx             ; Transmit the output from the last sample period.
        move    y:RightOutput,x0        ; 
        move    x0,x:RightTx            ; Transmit the output from the last sample period.

        move    x:LeftRx,a              ; receive left
        move    x:RightRx,b             ; receive right

        move    a,y:LeftInput           ; Save Current Left  input sample
        move    b,y:RightInput          ; Save Current Right input sample
    
    ;Process Left Channel   (left input sample is in a)        
        move    y:LeftInput,a
        bsr     ComputeSampleL           ; input in a, output in a
        move    a,y:LeftOutput

    ;Process Right Channel
    ; copy LeftOutput
        move    y:RightInput,a
        bsr     ComputeSampleR           ; input in a, output in a
        move    a,y:RightOutput
       
    ;Note: r5 and n5 are currently reserved for Host Interrupts

    ;Use Bottom layer of FootSwitch to turn on/off analog bypass
    ;   take the momentary footswitch signal (pulse) and generate a latched (step) control signal
        move    x:FootSwitch_TopLayer,a         ; load unmodified footswitch input
        move    x:FootSwitch_TopLayer,x1        ; load unmodified footswitch input 
        not     a       x:FootLatchMem,x0       ; not a  (also, load the previous momentary signal into x0)
        and     x0,a    x1,x:FootLatchMem       ; x0 & a (also, store current momentary signal for next iteration)
        move    x:FootLatch,x0                  ; load previous latched value
        eor     x0,a                            ; generate current latched (step) control signal
        move    a1,x:FootLatch                  ; store latched footswitch signal created from momentary footswitch
        
        move    x:FootLatch,a           ; load latching footswitch signal
        jclr    #0,a,ANALOG_BYPASS      ; bit clear = bypass

        movep   #>$000008,x:M_PDRB      ; bit 3 on  = FX_ON
        bset    #0,x:LED_Green          ; turn led on to indicate effect on

        jmp     END_ANALOG_BYPASS

ANALOG_BYPASS:            
        movep   #>$000004,x:M_PDRB      ; bit 2 on  = DIRECT_ON
        bclr    #0,x:LED_Green          ; turn led off to indicate effect off

END_ANALOG_BYPASS:

        rti                             ; return from interrupt

    ;Notes: The DSP path has a +3dB of gain applied in analog at the DSP output;
    ;       this gain was intended for cases where more output headroom was needed.
    ;       I am not normalizing for this output gain in this example code.
    ;
    ;       Port C contains sense lines for the input and output.  You may
    ;       use these to alter your algorithms to behave differently in various
    ;       mono/stereo input/output configurations if you wish.
    ;
    ;       Port B allows the DSP digital control over the following analog
    ;       sections of the Dock board: DSP path Pre-Emphasis filtering, 
    ;       DSP path De-Emphais filtering, DSP Patch Enable, and Dry Path enable.
    ;       In this example code, I leave the filtering off and use the
    ;       dry path for analog bypass. 

AccessSRAMWord0 equ $982245 ; 1 0 011 00 0 00100 0 100100 01 01
        ; DMA Control Register for channel 0
        ; bit(s)value description
        ; [23]    = DE       = 1 DMA Operation enabled (Trigger DMA transfer)
        ; [22]    = DIE      = 0 DMA Interrupt disabled
        ; [21:19] = DTM[2:0] = 011 triggered by DE, DE=0 after done
        ; [18:17] = DPR[1:0] = 00 priority level 0
        ; [16]    = DCON     = 0 Continuous mode disabled
        ; [15:11] = DRS[4:0] = 00100 Transfer done from channel 0
        ; [10]    = D3D      = 0 non 3-d mode
        ; [9:4]   = DAM[5:0] = 100100 no update s/d
        ; [3:2]   = DDS[1:0] = 01 Y memory destination
        ; [1:0]   = DSS[1:0] = 01 Y memory source

AccessSRAMWord1	equ $982A45 ; 1 0 011 00 0 00101 0 100100 01 01
        ; DMA Control Register for channel 1
        ; bit(s)value description
        ; [23]    = DE       = 1 DMA Operation enabled (Trigger DMA transfer)
        ; [22]    = DIE      = 0 DMA Interrupt disabled
        ; [21:19] = DTM[2:0] = 011 triggered by DE, DE=0 after done
        ; [18:17] = DPR[1:0] = 00 priority level 0
        ; [16]    = DCON     = 0 Continuous mode disabled
        ; [15:11] = DRS[4:0] = 00101 Transfer done from channel 1
        ; [10]    = D3D      = 0 non 3-d mode
        ; [9:4]   = DAM[5:0] = 100100 no update s/d
        ; [3:2]   = DDS[1:0] = 01 Y memory destination
        ; [1:0]   = DSS[1:0] = 01 Y memory source

; For the sample rate of 40 KHz, this SRAM provides 4.37 seconds of delay time.
; (4.37 seconds = ((512 * 1024 words) / 3 bytes per sample) / 40000 samples/second)
; External Memory from: 0x200000 to: 0x27FFFF

rdfx    MACRO   REG,K
        move    a,y:pacc
        move    REG,x0
        sub     x0,a
        move    a,x0
        move    #K,x1
        move    REG,a
        mac     x0,x1,a

        move    a,x:Debug_Read_from_DSP_1
        move    y:pacc,x0
        sub     x0,a
        move    a,x:Debug_Read_from_DSP_2
        move    x:Debug_Read_from_DSP_1,a
        ENDM

wrlx    MACRO   REG,K
        move    a,REG
        move    a,x0
        move    y:pacc,a
        sub     x0,a
        move    x0,y:pacc
        move    a,x0
        move    #K,x1
        move    y:pacc,a
        mac     x0,x1,a

        move    a,x:Debug_Read_from_DSP_3
        move    y:pacc,x0
        sub     x0,a
        move    a,x:Debug_Read_from_DSP_4
        move    x:Debug_Read_from_DSP_3,a
        ENDM

wrax    MACRO   REG,K
        move    a,REG
        move    a,x0
        move    %K,x1
        mpy     x0,x1,a
        ENDM

LowPassRC       MACRO   K
        move    y:efil,x0
        move    #K,x1
        sub     x0,a
        move    a,y1
        mac     y1,x1,a
        add     x0,a
        move    a,y:efil

        ;rdfx    y:efil,K
        ;wrlx    y:efil,-1.0
        ENDM

ReadFromSRAM MACRO SRC
        movep   SRC,x:M_DSR0
        nop
        nop
        jclr    #0,x:M_DSTR,*

        movep   #AccessSRAMWord0,x:M_DCR0
        nop
        nop
        jclr    #0,x:M_DSTR,*

        ENDM

WriteToSRAM  MACRO SRC
        movep   SRC,x:M_DDR1
        nop
        nop
        jclr    #1,x:M_DSTR,*

        movep   #AccessSRAMWord1,x:M_DCR1
        nop
        nop
        jclr    #1,x:M_DSTR,*

        ENDM

setup:
        ; Init DMA
        move    #>$000000,x0
        movep   x0,x:M_DCO0
        movep   x0,x:M_DCO1
        movep   #Chan0IO,x:M_DDR0
        movep   #Chan1IO,x:M_DSR1

        ; initiate Delay Buffer 
        move    x0,y:Chan0IO
        move    x0,y:Chan1IO

        move    #DelayBuffer,r0
        move    #3,n0
        move    #>$ffffff,m0

        move    #>BufferSize,b

InitMemory
        WriteToSRAM r0
        lua     (r0)+n0,r0
        sub     #>1,b
        bne     InitMemory
 
        move    #DelayBuffer,r0
        rts

ComputeSampleL:
        ; DMA Read past sample
        ReadFromSRAM    r0

        ; Calculate Input + Delay
        move    a,y0            ; Save input

        ; Calculate Input + Feedback
        move    x:Knob_3,x0
        move    y:Chan0IO,y1
        mac     x0,y1,a

        move    x:Switch_1,b
        cmp     #>1,b
        
        jlt     dry

        jeq     minus

K2      equ     -0.24596643992295197

        LowPassRC       K2

minus:
        LowPassRC       K2

dry:    move    a,y:Chan1IO

        ; DMA Write
        WriteToSRAM     r0

        move    x:Knob_1,x0     ; Output = input * (1 - Knob_1) + delay * Knob_1
        move    #1.0,a
        sub     x0,a
        move    a,x0
        mpy     x0,y0,a
        move    a,x0
        add     x0,a

        move    x:Knob_1,x0
        move    y:Chan0IO,x1
        mpy     x0,x1,b
        move    b,x0
        add     x0,b
        add     b,a

        ; Calculate next address

GetNextDelayIndex:
        lua	(r0)+n0,r0

        move    x:Knob_2,x0
        move    x0,y:DelayPotValue

        clr     b
        move    #(3*BufferSize-1),x1
        mac     x0,x1,b
        add     #>DelayBuffer,b

        move    r0,x0
        cmp     x0,b
        jge     GN1

        move    #DelayBuffer,r0
GN1:
        move    r0,y:DelaySize
        rts

ComputeSampleR:
        clr     a
        rts
