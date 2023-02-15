#ifndef __SYSTICK_H
#define __SYSTICK_H

#include "stm32f4xx.h" 

void Systick_Init(unsigned char clk);
void Delay_Us(unsigned int us);
void Delay_ms(unsigned int ms);
#endif
