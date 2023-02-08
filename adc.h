#ifndef  _ADC_H
#define  _ADC_H

#include "stm32f4xx.h"
#include "io_bit.h"


void Adc1_Init(void);
unsigned short Get_value(int ch);

#endif
