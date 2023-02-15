#ifndef __SYSTICK_H
#define __SYSTICK_H

#include "io_bit.h" 

#define ENABLED_RTOS 1

void Systick_Init(unsigned char clk);
void Delay_Us(unsigned int us);
void Delay_Ms(unsigned int Ms);

#endif
