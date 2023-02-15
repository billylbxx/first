#ifndef __ADC_H
#define __ADC_H
#include "io_bit.h" 

void ADC1_Init(void);
unsigned short get_value(int ch);
unsigned short get2_value(int ch);
char filter(int N,int ch);

#endif
