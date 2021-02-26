#pragma once

void ADC_init();
void ADC_start(uint8_t ch);
uint8_t ADC_isComplete();
uint16_t ADC_read();
uint16_t start_readChannel(uint8_t ch);