#ifndef __SYSTICK_H
#define __SYSTICK_H

#include "io_bit.h" 

void Systick_Inti(unsigned char clk);
void Delay_Us(unsigned int us);
void Delay_Ms(unsigned int Ms);
void delay_ms( int ms);
#endif
