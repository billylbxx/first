#ifndef __ADC_H
#define __ADC_H

#include "io_bit.h"

#define NTC_CH 10
#define IR_CH  11

void Adc_Init(void);
unsigned short Get_AdcValue(unsigned char ch);
unsigned short Get_AverageValue(unsigned char ch);

#endif
