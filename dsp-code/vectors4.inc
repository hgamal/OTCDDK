;
        page    132,60
;****************************************************************************
;   VECTORS.ASM
;   Vector table for the 56362                                        
;
;   Copyright (c) MOTOROLA 1997
;            Semiconductor Products Sector 
;            Digital Signal Processing Division
;
;****************************************************************************
;
         ORG       P:0

vectors  JMP       START                        ;- Hardware RESET                  

         jmp       *    
         NOP                                    ;- Stack Error

         jmp       *    
         NOP                                    ;- Illegal Opcode Interrupt     

         jmp       *    
         NOP                                    ;- Debug Request Interrupt     

         jmp       *    
         NOP                                    ;- Trap                        

         jmp       *    
         NOP                                    ;- NMI                         

         NOP                                    ;- Reserved                    
         NOP

         NOP                                    ;- Reserved                    
         NOP

;ba         jsr       IRQ_A                        ;- IRQA                            
        jmp     *
        nop

;ba         jsr       IRQ_B                        ;- IRQB                            
        jmp     *
        nop

;ba         jsr       IRQ_C                        ;- IRQC                        
        jmp     *
        nop

;ba         jsr       IRQ_D                        ;- IRQD
        jmp     *
        nop

         jmp       *    
         NOP                                    ;- DMA Channel 0               
;
         jmp       *    
         NOP                                    ;- DMA Channel 1                
;
         jmp       *    
         NOP                                    ;- DMA Channel 2                
;
         jmp       *    
         NOP                                    ;- DMA Channel 3                
;
         jmp       *    
         NOP                                    ;- DMA Channel 4                
;
         jmp       *    
         NOP                                    ;- DMA Channel 5                
;
         jmp    *                               ;- reserved
         nop
;
         jmp    *                               ;- reserved
         nop
;
         jmp    *                               ;- DAX underrun error
         nop
;
         jmp    *                               ;- DAX Block Transferred
         nop
;
         jmp    *                               ;- reserved
         nop
;
         jmp    *                               ;- DAX Audio Data Empty
         nop
;
         jsr    esai_rx_isr                     ; ESAI Receive data

         jsr    esai_rxeven_isr                 ; ESAI Receive data Even Slot (Right Channel)
;;         jmp    *                               ;MLS 12/20/97
;;         nop

         jsr    esai_rxe_isr                    ;ESAI Receive Data with exception
;         jmp    *
;         nop                                     ;MLS 12/20/97

;;        jsr     esai_rxls_isr
         jmp    *
         nop

;;         jsr    esai_tx_isr                     ;ESAI Transmit Data
		jmp    *
		nop

         jmp    *                               ;ESAI Transmit Even data
         nop
;
;;;         jsr    esai_txe_isr                    ;ESAI Transmit Data with exception
         jmp    *
         nop                                     ;MLS 12/20/97

;
;;;         jsr    esai_txls_isr                   ;ESAI Transmit Last slot
         jmp    *
         nop                                     ;MLS 12/20/97

;
        jmp     *                               ;SHI Transmit Data
        nop
;
        jmp     *                               ;SHI Transmit Underrun Error
        nop
;
		jsr		shi_receive						;SHI Receive FIFO Not Empty
;;        jmp     *                               ;SHI Receive FIFO Not Empty
;;        nop
;
         NOP                                     ;- Reserved                    
         NOP

;
;;;		jsr		shi_receive						;SHI Receive FIFO Full
        jmp     *                               ;SHI Receive FIFO Full
        nop
;
        jmp     *                               ;SHI Receive Overrun Error
        nop
;
        jmp     *                               ;SHI Bus Error
        nop

;MoreSpace         NOP                                     ;- Reserved             P:$4E
;MoreSpace         NOP
;MoreSpace;
;MoreSpace         NOP                                     ;- Reserved
;MoreSpace         NOP
;MoreSpace;
;MoreSpace         NOP                                     ;- Reserved                      
;MoreSpace         NOP
;MoreSpace
;MoreSpace         jmp       *    
;MoreSpace         NOP                        ;- Timer 0 Compare             
;MoreSpace;
;MoreSpace         jmp       *    
;MoreSpace         NOP                        ;- Timer 0 Overflow            
;MoreSpace;
;MoreSpace         jmp       *    
;MoreSpace         NOP                        ;- Timer 1 Compare             
;MoreSpace;
;MoreSpace         jmp       *    
;MoreSpace         NOP                        ;- Timer 1 Overflow            
;MoreSpace;
;MoreSpace         jmp       *    
;MoreSpace         NOP                        ;- Timer 2 Compare             
;MoreSpace; 
;MoreSpace         jmp       *    
;MoreSpace         NOP                        ;- Timer 2 Overflow            
;
