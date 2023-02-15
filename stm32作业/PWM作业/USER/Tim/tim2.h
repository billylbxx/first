#ifndef __TIM2_H
#define __TIM2_H
#include "io_bit.h" 

void Servo_SetAngle(unsigned char angle);
void tim2_Init(int ARR,int PSC);
void lcd_ld(unsigned char CCR);
#endif
