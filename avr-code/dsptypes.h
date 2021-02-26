#ifndef _DSPTYPES_H
#define _DSPTYPES_H

typedef unsigned int byte;
typedef unsigned int word;

typedef union {
  struct {
  byte _Byte0;
  }Merge_uint8_t;
  struct {
    byte _bit0       :1;              /*  Bit 0  */
    byte _bit1       :1;              /*  Bit 1  */
    byte _bit2       :1;              /*  Bit 2  */
    byte _bit3       :1;              /*  Bit 3  */
    byte _bit4       :1;              /*  Bit 4  */
    byte _bit5       :1;              /*  Bit 5  */
    byte _bit6       :1;              /*  Bit 6  */
    byte _bit7       :1;              /*  Bit 7  */
  } Bits;
} word8;

typedef union {
  struct {
  byte _Byte0;
  }Merge_uint8_t;
  struct {
    byte DebugDataFromPC       :1;              /*  Bit 0  */
    byte ReadyForBoot          :1;              /*  Bit 1  */
    byte SendDataToDSP         :1;              /*  Bit 2  */
    byte DataSent              :1;              /*  Bit 3  */
    byte _bit4       :1;              /*  Bit 4  */
    byte _bit5       :1;              /*  Bit 5  */
    byte _bit6       :1;              /*  Bit 6  */
    byte _bit7       :1;              /*  Bit 7  */
  } Bits;
} u8DebugFlag;



/**********************************************  16 Bits *************************************************************/
typedef union {
  struct {
      word _Word0;
  } Merge_uint16_t;  
  struct {
      byte _Byte1;
      byte _Byte0;
  } Merge_uint8_t;
  struct {
    byte _bit8       :1;              /*  Bit 8  */
    byte _bit9       :1;              /*  Bit 9  */
    byte _bit10      :1;              /*  Bit 10 */
    byte _bit11      :1;              /*  Bit 11 */
    byte _bit12      :1;              /*  Bit 12 */
    byte _bit13      :1;              /*  Bit 13 */
    byte _bit14      :1;              /*  Bit 14 */
    byte _bit15      :1;              /*  Bit 15 */
    
    byte _bit0       :1;              /*  Bit 0  */
    byte _bit1       :1;              /*  Bit 1  */
    byte _bit2       :1;              /*  Bit 2  */
    byte _bit3       :1;              /*  Bit 3  */
    byte _bit4       :1;              /*  Bit 4  */
    byte _bit5       :1;              /*  Bit 5  */
    byte _bit6       :1;              /*  Bit 6  */
    byte _bit7       :1;              /*  Bit 7  */
  } Bits;
} word16;


/**********************************************  24 Bits *************************************************************/
typedef union {
  struct {
      byte _Byte2;
      byte _Byte1;
      byte _Byte0;
  } Merge_uint8_t;
  struct {
    byte _bit16      :1;              /*  Bit 16 */
    byte _bit17      :1;              /*  Bit 17 */
    byte _bit18      :1;              /*  Bit 18 */
    byte _bit19      :1;              /*  Bit 19 */
    byte _bit20      :1;              /*  Bit 20 */
    byte _bit21      :1;              /*  Bit 21 */
    byte _bit22      :1;              /*  Bit 22 */
    byte _bit23      :1;              /*  Bit 23 */
    
    byte _bit8       :1;              /*  Bit 8  */
    byte _bit9       :1;              /*  Bit 9  */
    byte _bit10      :1;              /*  Bit 10 */
    byte _bit11      :1;              /*  Bit 11 */
    byte _bit12      :1;              /*  Bit 12 */
    byte _bit13      :1;              /*  Bit 13 */
    byte _bit14      :1;              /*  Bit 14 */
    byte _bit15      :1;              /*  Bit 15 */
    
    byte _bit0       :1;              /*  Bit 0  */
    byte _bit1       :1;              /*  Bit 1  */
    byte _bit2       :1;              /*  Bit 2  */
    byte _bit3       :1;              /*  Bit 3  */
    byte _bit4       :1;              /*  Bit 4  */
    byte _bit5       :1;              /*  Bit 5  */
    byte _bit6       :1;              /*  Bit 6  */
    byte _bit7       :1;              /*  Bit 7  */
  } Bits;

} word24;

/**********************************************  32 Bits *************************************************************/
typedef union {
  struct {
      long  _Long0;
  }Merge_uint32_t;
  struct {
      word _Word1;
      word _Word0;
  } Merge_uint16_t;
  struct {
      byte _Byte3;
      byte _Byte2;
      byte _Byte1;
      byte _Byte0;
  } Merge_uint8_t;
  struct {
    byte _bit24      :1;              /*  Bit 24 */
    byte _bit25      :1;              /*  Bit 25 */
    byte _bit26      :1;              /*  Bit 26 */
    byte _bit27      :1;              /*  Bit 27 */
    byte _bit28      :1;              /*  Bit 28 */
    byte _bit29      :1;              /*  Bit 29 */
    byte _bit30      :1;              /*  Bit 30 */
    byte _bit31      :1;              /*  Bit 31 */
    
    byte _bit16      :1;              /*  Bit 16 */
    byte _bit17      :1;              /*  Bit 17 */
    byte _bit18      :1;              /*  Bit 18 */
    byte _bit19      :1;              /*  Bit 19 */
    byte _bit20      :1;              /*  Bit 20 */
    byte _bit21      :1;              /*  Bit 21 */
    byte _bit22      :1;              /*  Bit 22 */
    byte _bit23      :1;              /*  Bit 23 */
    
    byte _bit8       :1;              /*  Bit 8  */
    byte _bit9       :1;              /*  Bit 9  */
    byte _bit10      :1;              /*  Bit 10 */
    byte _bit11      :1;              /*  Bit 11 */
    byte _bit12      :1;              /*  Bit 12 */
    byte _bit13      :1;              /*  Bit 13 */
    byte _bit14      :1;              /*  Bit 14 */
    byte _bit15      :1;              /*  Bit 15 */
    
    byte _bit0       :1;              /*  Bit 0  */
    byte _bit1       :1;              /*  Bit 1  */
    byte _bit2       :1;              /*  Bit 2  */
    byte _bit3       :1;              /*  Bit 3  */
    byte _bit4       :1;              /*  Bit 4  */
    byte _bit5       :1;              /*  Bit 5  */
    byte _bit6       :1;              /*  Bit 6  */
    byte _bit7       :1;              /*  Bit 7  */
  } Bits;
  struct {
    uint8_t  _Byte3;
    uint16_t _MidWord;
    uint8_t  _Byte0;
  } Merge_MIDuint16_t;
} t_uint32_t;
/******************************* Bit definitions *******************************************************/
#define bit0                      Bits._bit0
#define bit1                      Bits._bit1
#define bit2                      Bits._bit2
#define bit3                      Bits._bit3
#define bit4                      Bits._bit4
#define bit5                      Bits._bit5
#define bit6                      Bits._bit6
#define bit7                      Bits._bit7
#define bit8                      Bits._bit8 
#define bit9                      Bits._bit9 
#define bit10                     Bits._bit10
#define bit11                     Bits._bit11
#define bit12                     Bits._bit12
#define bit13                     Bits._bit13
#define bit14                     Bits._bit14
#define bit15                     Bits._bit15
#define bit16                     Bits._bit16
#define bit17                     Bits._bit17
#define bit18                     Bits._bit18
#define bit19                     Bits._bit19
#define bit20                     Bits._bit20
#define bit21                     Bits._bit21
#define bit22                     Bits._bit22
#define bit23                     Bits._bit23
#define bit24                     Bits._bit24
#define bit25                     Bits._bit25
#define bit26                     Bits._bit26
#define bit27                     Bits._bit27
#define bit28                     Bits._bit28
#define bit29                     Bits._bit29
#define bit30                     Bits._bit30
#define bit31                     Bits._bit31
/******************************* Byte definitions *******************************************************/
#define Byte0                    Merge_uint8_t._Byte0
#define Byte1                    Merge_uint8_t._Byte1
#define Byte2                    Merge_uint8_t._Byte2
#define Byte3                    Merge_uint8_t._Byte3
/******************************* Word definitions *******************************************************/
#define Word0                    Merge_uint16_t._Word0
#define Word1                    Merge_uint16_t._Word1

/******************************* Word definitions *******************************************************/
#define Word24b                  Merge_uint16_t._Word0
#define Word1                    Merge_uint16_t._Word1


/******************************* Long definitions *******************************************************/
#define Long0                    Merge_uint32_t._Long0


#define bit0_MASK                 1
#define bit1_MASK                 2
#define bit2_MASK                 4
#define bit3_MASK                 8
#define bit4_MASK                 16
#define bit5_MASK                 32
#define bit6_MASK                 64
#define bit7_MASK                 128



#endif /* _VAR_TEMPLATE_ */