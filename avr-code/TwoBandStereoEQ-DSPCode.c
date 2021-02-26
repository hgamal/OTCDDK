#include <stdint.h>

/*
    This is the code that will be downloaded to the DSP.
    You can modify this constant based on the dumped file displayed by the GUI.
    Make sure you change the code size (first 3 bytes in the array)
*/
        
const uint8_t DSP_Code[] = {
    0x00, 0x01, 0x60,    /* This is the size of the code in words*/
    0x00, 0x00, 0x00,    /* This is the starting address of RAM where the code will be stored */

    /*
       This is the compiled and dumped code. You can use the GUI to obtain this array of bytes.
       Be sure to paste numbers as bytes, and to modify the size of the code based on what is displayed on the GUI
    */


0x0A, 0xF0, 0x80, 0x00, 0x00, 0x4E, 0x0C, 0x00, 0x02, 0x00, 0x00, 0x00, 0x0C, 0x00, 0x04, 0x00, 0x00, 0x00, 0x0C, 0x00, 0x06, 0x00, 0x00, 0x00, 0x0C, 0x00, 0x08, 0x00, 0x00, 0x00, 
0x0C, 0x00, 0x0A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x00, 0x10, 0x00, 0x00, 0x00, 0x0C, 0x00, 0x12, 0x00, 0x00, 0x00, 
0x0C, 0x00, 0x14, 0x00, 0x00, 0x00, 0x0C, 0x00, 0x16, 0x00, 0x00, 0x00, 0x0C, 0x00, 0x18, 0x00, 0x00, 0x00, 0x0C, 0x00, 0x1A, 0x00, 0x00, 0x00, 0x0C, 0x00, 0x1C, 0x00, 0x00, 0x00, 
0x0C, 0x00, 0x1E, 0x00, 0x00, 0x00, 0x0C, 0x00, 0x20, 0x00, 0x00, 0x00, 0x0C, 0x00, 0x22, 0x00, 0x00, 0x00, 0x0C, 0x00, 0x24, 0x00, 0x00, 0x00, 0x0C, 0x00, 0x26, 0x00, 0x00, 0x00, 
0x0C, 0x00, 0x28, 0x00, 0x00, 0x00, 0x0C, 0x00, 0x2A, 0x00, 0x00, 0x00, 0x0C, 0x00, 0x2C, 0x00, 0x00, 0x00, 0x0C, 0x00, 0x2E, 0x00, 0x00, 0x00, 0x0B, 0xF0, 0x80, 0x00, 0x00, 0xC2, 
0x0B, 0xF0, 0x80, 0x00, 0x00, 0xC7, 0x0B, 0xF0, 0x80, 0x00, 0x00, 0xBB, 0x0C, 0x00, 0x36, 0x00, 0x00, 0x00, 0x0C, 0x00, 0x38, 0x00, 0x00, 0x00, 0x0C, 0x00, 0x3A, 0x00, 0x00, 0x00, 
0x0C, 0x00, 0x3C, 0x00, 0x00, 0x00, 0x0C, 0x00, 0x3E, 0x00, 0x00, 0x00, 0x0C, 0x00, 0x40, 0x00, 0x00, 0x00, 0x0C, 0x00, 0x42, 0x00, 0x00, 0x00, 0x0B, 0xF0, 0x80, 0x00, 0x00, 0x97, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x00, 0x48, 0x00, 0x00, 0x00, 0x0C, 0x00, 0x4A, 0x00, 0x00, 0x00, 0x0C, 0x00, 0x4C, 0x00, 0x00, 0x00, 
0x00, 0x03, 0xF8, 0x08, 0xF4, 0xBD, 0x2D, 0x00, 0x63, 0x0A, 0xFA, 0x6E, 0x07, 0xF4, 0x34, 0x00, 0x00, 0x40, 0x07, 0xF4, 0x36, 0xFC, 0xC3, 0x04, 0x07, 0xF4, 0x38, 0xFC, 0xC3, 0x04, 
0x07, 0xF4, 0x37, 0x70, 0x7D, 0x00, 0x07, 0xF4, 0x35, 0x02, 0x7D, 0x80, 0x07, 0xF4, 0x3D, 0x00, 0x00, 0x00, 0x07, 0xF4, 0x3F, 0x00, 0x0B, 0xF8, 0x07, 0xF4, 0x3E, 0x00, 0x0C, 0x7E, 
0x08, 0xF4, 0x8F, 0x00, 0x00, 0x00, 0x08, 0xF4, 0x8E, 0x00, 0x00, 0x0F, 0x08, 0xF4, 0x8D, 0x00, 0x00, 0x08, 0x07, 0xF4, 0x3B, 0x00, 0x00, 0x03, 0x07, 0xF4, 0x3C, 0x00, 0x00, 0x00, 
0x07, 0xF4, 0x39, 0x00, 0x00, 0x03, 0x07, 0xF4, 0x3A, 0x00, 0x00, 0x00, 0x07, 0xF4, 0x20, 0x00, 0x00, 0x00, 0x01, 0x37, 0x20, 0x01, 0x35, 0x20, 0x08, 0xF4, 0xB9, 0x24, 0x06, 0xB5, 
0x08, 0xF4, 0xB8, 0x20, 0x03, 0xB1, 0x08, 0xF4, 0xBB, 0x01, 0x24, 0xA5, 0x07, 0xF4, 0x10, 0x00, 0x30, 0x01, 0x07, 0xF4, 0x11, 0x00, 0x11, 0x89, 0x44, 0xF4, 0x00, 0x00, 0x00, 0x00, 
0x20, 0x90, 0x00, 0x06, 0x1A, 0xA0, 0x44, 0x58, 0x00, 0x44, 0xF4, 0x00, 0x40, 0x00, 0x00, 0x44, 0x00, 0x00, 0x44, 0x01, 0x00, 0x44, 0x02, 0x00, 0x44, 0x03, 0x00, 0x44, 0x04, 0x00, 
0x44, 0x05, 0x00, 0x44, 0xF4, 0x00, 0x00, 0x00, 0x00, 0x05, 0xF4, 0x25, 0x00, 0xFF, 0xFF, 0x04, 0xC4, 0x93, 0x08, 0xF4, 0xBE, 0x00, 0x00, 0x07, 0x00, 0xFC, 0xB8, 0x07, 0xF4, 0x3D, 
0x00, 0x00, 0x02, 0x00, 0x00, 0x86, 0x0C, 0x00, 0x95, 0x07, 0x70, 0x14, 0x00, 0x00, 0x11, 0x07, 0xF4, 0x13, 0x00, 0x00, 0x00, 0x0B, 0x11, 0x20, 0x0D, 0x10, 0x48, 0x00, 0x00, 0x05, 
0x0B, 0x11, 0x23, 0x0D, 0x10, 0x48, 0x00, 0x00, 0x0D, 0x01, 0x91, 0x91, 0x00, 0x00, 0xA1, 0x04, 0x55, 0x94, 0x07, 0xF4, 0x13, 0x00, 0x00, 0x00, 0x01, 0x91, 0x91, 0x00, 0x00, 0xA6, 
0x07, 0x65, 0x14, 0x07, 0xF4, 0x13, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x01, 0x91, 0x91, 0x00, 0x00, 0xAC, 0x04, 0x55, 0x94, 0x07, 0xF4, 0x13, 0x00, 0x00, 0x00, 0x01, 0x91, 0x91, 
0x00, 0x00, 0xB1, 0x04, 0x5D, 0x94, 0x07, 0xE5, 0x13, 0x01, 0x91, 0x91, 0x00, 0x00, 0xB5, 0x04, 0x5D, 0x94, 0x07, 0xF4, 0x13, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x01, 0x33, 0x07, 
0x01, 0x33, 0x0E, 0x07, 0x70, 0x28, 0x00, 0x00, 0x0D, 0x07, 0xF0, 0x20, 0x00, 0x00, 0x0F, 0x00, 0x00, 0x04, 0x07, 0x70, 0x28, 0x00, 0x00, 0x0D, 0x07, 0xF0, 0x20, 0x00, 0x00, 0x0F, 
0x00, 0x00, 0x04, 0x07, 0x70, 0x28, 0x00, 0x00, 0x0C, 0x07, 0xF0, 0x20, 0x00, 0x00, 0x0E, 0x0D, 0x10, 0x80, 0x00, 0x00, 0x5C, 0x4C, 0x81, 0x00, 0x44, 0x0E, 0x00, 0x4C, 0x83, 0x00, 
0x44, 0x0F, 0x00, 0x56, 0x8C, 0x00, 0x57, 0x8D, 0x00, 0x5E, 0x00, 0x00, 0x5F, 0x02, 0x00, 0x66, 0xF4, 0x00, 0x00, 0x00, 0x06, 0x64, 0xF4, 0x00, 0x00, 0x00, 0x1A, 0x63, 0xF4, 0x00, 
0x00, 0x00, 0x04, 0x0D, 0x10, 0x80, 0x00, 0x00, 0x1E, 0x5E, 0x01, 0x00, 0x5E, 0x82, 0x00, 0x64, 0xF4, 0x00, 0x00, 0x00, 0x1A, 0x63, 0xF4, 0x00, 0x00, 0x00, 0x08, 0x0D, 0x10, 0x80, 
0x00, 0x00, 0x16, 0x5E, 0x03, 0x00, 0x56, 0x89, 0x00, 0x45, 0x89, 0x00, 0x44, 0xA1, 0x17, 0x45, 0x21, 0x46, 0x44, 0xA0, 0x00, 0x20, 0x00, 0x43, 0x54, 0x20, 0x00, 0x56, 0xA0, 0x00, 
0x0A, 0xCE, 0x00, 0x00, 0x00, 0xF5, 0x08, 0xF4, 0x8D, 0x00, 0x00, 0x08, 0x0A, 0x0B, 0x20, 0x0A, 0xF0, 0x80, 0x00, 0x00, 0xF8, 0x08, 0xF4, 0x8D, 0x00, 0x00, 0x04, 0x0A, 0x0B, 0x00, 
0x00, 0x00, 0x04, 0x0D, 0x10, 0x80, 0x00, 0x00, 0x23, 0x56, 0xDE, 0x00, 0x0A, 0xCE, 0x21, 0x00, 0x01, 0x02, 0x0A, 0xCE, 0x20, 0x00, 0x01, 0x06, 0x0A, 0xF0, 0x80, 0x00, 0x01, 0x07, 
0x20, 0x8F, 0x00, 0x21, 0xE6, 0x00, 0x0A, 0xF0, 0x80, 0x00, 0x01, 0x07, 0x21, 0xE6, 0x00, 0x20, 0x00, 0xE0, 0x0C, 0x1D, 0x04, 0x20, 0x00, 0x70, 0x0D, 0x10, 0x80, 0x00, 0x00, 0x12, 
0x56, 0xD6, 0x00, 0x0A, 0xCE, 0x21, 0x00, 0x01, 0x13, 0x0A, 0xCE, 0x20, 0x00, 0x01, 0x17, 0x0A, 0xF0, 0x80, 0x00, 0x01, 0x18, 0x20, 0x8F, 0x00, 0x21, 0xE6, 0x00, 0x0A, 0xF0, 0x80, 
0x00, 0x01, 0x18, 0x21, 0xE6, 0x00, 0x20, 0x00, 0xE0, 0x0C, 0x1D, 0x04, 0x20, 0x00, 0x70, 0x00, 0x00, 0x0C, 0x5F, 0xDB, 0x00, 0xE4, 0xFC, 0x00, 0x44, 0xDC, 0xEA, 0x21, 0xC7, 0xD4, 
0x5F, 0x63, 0xC2, 0x20, 0x00, 0x32, 0x20, 0xCF, 0x14, 0x21, 0xC4, 0x00, 0xF4, 0xFC, 0xAA, 0x5F, 0x5B, 0x00, 0x00, 0x00, 0x0C, 0x44, 0x80, 0x00, 0x56, 0xF4, 0x00, 0x01, 0x07, 0x89, 
0x46, 0xF4, 0x00, 0x65, 0xEA, 0x0D, 0x20, 0x00, 0xD2, 0x56, 0x1A, 0x00, 0x56, 0x16, 0x00, 0x44, 0x83, 0x00, 0x56, 0xF4, 0x00, 0x01, 0x07, 0x89, 0x20, 0x00, 0xD2, 0x56, 0x1D, 0x00, 
0x56, 0x17, 0x00, 0x44, 0x81, 0x00, 0x46, 0xF4, 0x00, 0x86, 0x66, 0x67, 0x56, 0xF4, 0x00, 0x7F, 0xFF, 0xFF, 0x20, 0x00, 0xD2, 0x56, 0x1B, 0x00, 0x56, 0x18, 0x00, 0x44, 0x84, 0x00, 
0x56, 0xF4, 0x00, 0x7F, 0xFF, 0xFF, 0x20, 0x00, 0xD2, 0x56, 0x1E, 0x00, 0x56, 0x19, 0x00, 0x56, 0x82, 0x00, 0x44, 0xF4, 0x00, 0x40, 0x00, 0x00, 0x20, 0x00, 0x44, 0x01, 0x40, 0x85, 
0x0A, 0xF0, 0xA9, 0x00, 0x01, 0x4D, 0x20, 0x00, 0x32, 0x0A, 0xF0, 0x80, 0x00, 0x01, 0x50, 0x10, 0xB4, 0x00, 0x33, 0x33, 0x33, 0x20, 0x00, 0xD0, 0x56, 0x1C, 0x00, 0x56, 0x85, 0x00, 
0x44, 0xF4, 0x00, 0x40, 0x00, 0x00, 0x20, 0x00, 0x44, 0x01, 0x40, 0x85, 0x0A, 0xF0, 0xA9, 0x00, 0x01, 0x5B, 0x20, 0x00, 0x32, 0x0A, 0xF0, 0x80, 0x00, 0x01, 0x5E, 0x10, 0xB4, 0x00, 
0x33, 0x33, 0x33, 0x20, 0x00, 0xD0, 0x56, 0x1F, 0x00, 0x00, 0x00, 0x0C, 


};