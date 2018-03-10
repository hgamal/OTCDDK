                page    132,60
                include "..\Ioequ.inc"
                include "..\vectors4.inc"                
                list

;---------------------------------------------------------------------------------------------------
;   FilterExample.asm
;       This file is setup to work with the Line 6 / Freescale Tone Core Developer's Kit.
;       It is a simple 2 band stereo EQ with control over filter type, frequency, bandwidth
;       and shelving level for each band. 
;       
;   Line 6,  November 30, 2007      
;		Copyright (c) 2008 Line 6 Inc
;		Line 6 Confidential Information
;       
;---------------------------------------------------------------------------------------------------
;    Services performed by LINE 6 in this matter are performed AS IS and without any warranty.  
;    CUSTOMER retains the final decision relative to the total design and functionality of the     
;    end product. LINE 6 neither guarantees nor will be held liable by CUSTOMER for the         
;    success of this project. LINE 6 disclaims all warranties, express, implied or statutory    
;    including, but not limited to, implied warranty of merchantability or fitness for a           
;    particular purpose on any hardware, software ore advise supplied to the project by LINE 6, 
;    and or any product resulting from LINE 6 services . In no event shall LINE 6 be         
;    liable for incidental or consequential damages arising out of this agreement. CUSTOMER        
;    agrees to hold LINE 6 harmless against any and all claims demands or actions by anyone     
;    on account of any damage, or injury, whether commercial, contractual, or tortuous, rising     
;    directly or indirectly as a result of the advise or assistance supplied CUSTOMER in           
;    connection with product, services or goods supplied under this Agreement.                     
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
Filter_1_K              ds  1       ; 1ah
Filter_1_Q              ds  1       ; 1bh
Filter_1_Shelve         ds  1       ; 1ch
                                      
Filter_2_K              ds  1       ; 1dh
Filter_2_Q              ds  1       ; 1eh
Filter_2_Shelve         ds  1       ; 1fh

FootLatch               ds  1       ; 20h
FootLatchMem            ds  1       ; 21h

        org     y:$000              
;...copy of current input and output samples
LeftInput               ds  1       ; 00h
LeftOutput              ds  1       ; 01h
RightInput              ds  1       ; 02h
RightOutput             ds  1       ; 03h


;...Filter memory
Filter_1_A_Left         ds  1       ; 04h
Filter_1_Y_Left         ds  1       ; 05h

Filter_2_A_Left         ds  1       ; 06h
Filter_2_Y_Left         ds  1       ; 07h

Filter_1_A_Right        ds  1       ; 08h
Filter_1_Y_Right        ds  1       ; 09h

Filter_2_A_Right        ds  1       ; 0ah
Filter_2_Y_Right        ds  1       ; 0bh


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
        movep   #$0124A5,x:M_BCR                ;        0001 0010 0100 0110 0011
                ;bus request hold off           (bit23=0)
                ;bus lock hold off              (bit22=0)
                ;bus state                      (bit21=0)
                ;default area wait states       (bit20:16=00001)
                ;area 3 wait states             (bit15:13=001)
                ;area 2 wait states             (bit12:10=001)
                ;area 1 wait states             (bit9:5=00011)
                ;area 0 wait states             (bit4:0=00011)


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
        movep   x:M_RX0,x:LeftRx        
        movep   x:LeftTx,x:M_TX0        

    ;Process the scaling of knob values to generate coefficients        
    ;   This is being called per sample, but could be done at a 
    ;   slower rate if bandwidth is needed
        bsr     Generate_Coeffs

        move    y:LeftOutput,x0         ; 
        move    x0,x:LeftTx             ; Transmit the output from the last sample period.
        move    y:RightOutput,x0        ; 
        move    x0,x:RightTx            ; Transmit the output from the last sample period.

        move    x:LeftRx,a              ; receive left
        move    x:RightRx,b             ; receive right

        move    a,y:LeftInput           ; Save Current Left  input sample
        move    b,y:RightInput          ; Save Current Right input sample
    
    ;Note: r5 and n5 are currently reserved for Host Interrupts

    ;Process Left Channel   (left input sample is in a)        
        move    #>Switch_1,r6           ; point to x:mem (switch states)
        move    #>Filter_1_K,r4         ; point to x:mem (coefficients)
        move    #>Filter_1_A_Left,r3    ; point to y:mem (filter mem) 
        bsr     TwoBand_EQ              ; input in a, output in a
        move    a,y:LeftOutput

    ;Process Right Channel  (r6 is currently pointing to Switch 1)         
        move    y:RightInput,a          ; load right input sample in a
        move    #>Filter_1_K,r4         ; point to x:mem (coefficients)
        move    #>Filter_1_A_Right,r3   ; point to y:mem (filter mem) 
        bsr     TwoBand_EQ              ; input in a, output in a
        move    a,y:RightOutput
       
    ;Use Bottom layer of FootSwitch to turn on/off analog bypass
    ;   take the momentary footswitch signal (pulse) and generate a latched (step) control signal
        move    x:FootSwitch_BottomLayer,a      ; load unmodified footswitch input
        move    x:FootSwitch_BottomLayer,x1     ; load unmodified footswitch input 
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
	;
	;		r5 and n5 are reserved by the Host Interrupt process
	;
	;


;XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
;XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
; Common Subroutines
;XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
;XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX


;-----------------------------------------------------------------------
;   TwoBand_EQ 
;       This sub routine processes two state variable filters a
;       with selectable filter type and shelving gain (-14dB to +14dB)
;
;       INPUT:      a  = audio input to be filtered
;                   r6 = pointer to switch state memory
;                   r4 = pointer to filter coefficients
;                   r3 = pointer to filter memory
;
;       OUTPUT:     a  = filtered output
;
;
TwoBand_EQ:
        bsr     SV_Filter               ; process the state variable filter
        ;select filter type
        move    x:(r6)+,a            
        jset    #1,a,Highpass_1_L       ; x0 = hp_output
        jset    #0,a,Bandpass_1_L       ; b  = bp_output
        jmp     EndSwitch_1_L           ; y0 = lp_output
Highpass_1_L:
        move    x0,b
        move    b,y0
        jmp     EndSwitch_1_L
Bandpass_1_L:
        move    b,y0
EndSwitch_1_L:
        mpy     x1,y0,a                 ; process shelving scalar
        asl     #2,a,a                  ; add gain to extend shelving range -14dB to +14dB       
        add     y1,a                    ; add filter input to create shelf

        bsr     SV_Filter               ; process the state variable filter
        ;select filter type
        move    x:(r6)-,a            
        jset    #1,a,Highpass_2_L       ; x0 = hp_output
        jset    #0,a,Bandpass_2_L       ; b  = bp_output
        jmp     EndSwitch_2_L           ; y0 = lp_output
Highpass_2_L:
        move    x0,b
        move    b,y0
        jmp     EndSwitch_2_L
Bandpass_2_L:
        move    b,y0
EndSwitch_2_L:
        mpy     x1,y0,a                 ; process shelving scalar
        asl     #2,a,a                  ; add gain to extend shelving range -14dB to +14dB       
        add     y1,a                    ; add filter input to create shelf  
        rts


;-----------------------------------------------------------------------
;   SV_Filter 
;       This sub routine is a state variable band-pass 
;       This filter outputs low-pass, band-pass, and high-pass filter types
;
;       INPUT:      a  = audio input to be filtered
;             x:(r4)+0 = frequency coefficient,  K = 2*pi*fc/Fs
;             x:(r4)+1 = bandwidth coefficient,  1/(2Q) where Q is bandwidth
;             x:(r4)+2 = shelving  coefficient  
;
;       OUTPUTS:    y0 = lp_output  
;                   b  = bp_output  
;                   x0 = hp_output  
;                   a  = hp_output  
;                   x1 = next coeff 
;                   y1 = input
;
SV_Filter:
        move    y:(r3)+,b                               ; b  = a(n-1)
        move    x:(r4)+,x1  y:(r3)-,y0                  ; x1 = K                        y0  = y(n-1)
        mac     x1,y0,b     x:(r4)+,x0                  ; b  = K * y(n-1) + a(n-1)      x0  = 1/2Q
        mpy     -y0,x0,a    a,y1                        ; a  = -y(n-1)/2Q               y1  = x(n)
        mac     y1,x0,a     b,y:(r3)                    ; a  = (x(n)-y(n-1))/2Q         store a(n)
        asl     a                                       ; a  = (x(n)-y(n-1))/Q
        sub     b,a         y0,b                        ; a  = (x(n)-y(n-1))/Q - a(n)   b  = y(n-1)
        move    a,x0                                    ; x0 = HP output
        mac     x1,x0,b     x:(r4)+,x1  y:(r3)+,y0      ; b  = k(HP) + y(n-1)           x1 = next coeff y0 = LP output
        move    b,y:(r3)+                               ; store next y(n-1) 
        rts 


;-----------------------------------------------------------------------
;   Generate_Coeffs 
;       This sub routine takes the knob values that range from a 24 bit 
;       two's compliment 0 to 1 (0x000000 to 0x7FFFFF) and converts them
;       for use as coefficients.
;
;       This is example code is setup so that Fs = 39062.5
;
Generate_Coeffs:
    ;Convert Knob 1 to Filter 1's Frequency
        move    x:Knob_1,x0                             ; load Knob 1's current value (range = 0 to 1)
        move    #>$010789,a                             ;  a =   50 Hz
        move    #>$65EA0D,y0                            ; y0 = 4950 Hz
            ; for frequency = 4950 Hz, the coefficient value will be 2*pi*4950/Fs = 0.796205242 
            ; that number is then multiplied by (2^23)-1 for 24 bit two's compliment = 6679053 decimal = 65EA0D hex       
        mac     x0,y0,a                                 ; a  =  4950 * value + 50; register a now ranges between 50 and 5000 Hz
        move    a,x:Filter_1_K                          ; store current frequency value
		move	a,x:Debug_Read_from_DSP_1				; DEBUG Example, send filter 1 frequency coefficient to GUI

    ;Convert Knob 4 to Filter 2's Frequency
        move    x:Knob_4,x0                             ; load Knob 4's value
        move    #>$010789,a                             ; a  =   50 Hz
        mac     x0,y0,a                                 ; a  =  4950 * value + 50
        move    a,x:Filter_2_K                          ; store current frequency value
		move	a,x:Debug_Read_from_DSP_2				; DEBUG Example, send filter 2 frequency coefficient to GUI
                        
    ;Convert Knob 2 to Filter 1's Q
        move    x:Knob_2,x0                             ; load Knob 2's value
        move    #>$866667,y0                            ; y0 = 1/2Q
        move    #>$7FFFFF,a                             ; a  = 1/2Q, where Q = .5
        mac     x0,y0,a                                 ; a  = 1/2Q, where Q ranges between .5 and 10
        move    a,x:Filter_1_Q                          ; store current 1/2Q value
		move	a,x:Debug_Read_from_DSP_3				; DEBUG Example, send filter 1 bandwidth to GUI

    ;Convert Knob 5 to Filter 2's Q
        move    x:Knob_5,x0                             ; load Knob 5's value
        move    #>$7FFFFF,a                             ; a  = 1/2Q, where Q = .5
        mac     x0,y0,a                                 ; a  = 1/2Q, where Q ranges between .5 and 10
        move    a,x:Filter_2_Q                          ; store current 1/2Q value
		move	a,x:Debug_Read_from_DSP_4				; DEBUG Example, send filter 2 bandwidth to GUI

    ;Convert Knob 3 to Filter 1's Shelving
        move    x:Knob_3,a                              ; load Knob 3's value (range = 0 to 1)
        move    #>$400000,x0                            ; x0 = .5
        sub     x0,a                                    ; (range = -.5 to .5)
        cmp     #0,a                                    ; compare range to zero
        jlt     CuttingShelve1                          ; jump if less than zero
        asl     a                                       ; (range = 0  to 1)
        jmp     DoneShelve1                             ; jump to dove shelve 1
CuttingShelve1                                          ; generate negative shelve range
        move    #>$333333,x0    a,y0                    ; x0 = .4
        mpy     x0,y0,a                                 ; (range = -.2 to 0)
DoneShelve1                                             ; mark end of shelve coeff curve generation
        ; coefficient range = -.2 to 1 for a finale shelve range of -14dB to +14dB
        move    a,x:Filter_1_Shelve                     ; store current shelving (range -.25 to 1)

    ;Convert Knob 6 to Filter 2's Shelving
        move    x:Knob_6,a                             ; load Knob 6's value (range = 0 to 1)
        move    #>$400000,x0                            ; x0 = .5
        sub     x0,a                                    ; (range = -.5 to .5)
        cmp     #0,a                                    ; compare range to zero
        jlt     CuttingShelve2                          ; jump if less than zero
        asl     a                                       ; (range = 0  to 1)
        jmp     DoneShelve2                             ; jump to dove shelve 1
CuttingShelve2                                          ; generate negative shelve range
        move    #>$333333,x0    a,y0                    ; x0 = .4
        mpy     x0,y0,a                                 ; (range = -.2 to 0)
DoneShelve2                                             ; mark end of shelve coeff curve generation
        ; coefficient range = -.2 to 1 for a finale shelve range of -14dB to +14dB
        move    a,x:Filter_2_Shelve                     ; store current shelving 
        rts

